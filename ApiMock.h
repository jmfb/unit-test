#pragma once
#if defined(_WINDOWS_)
#include "TestException.h"
#include <functional>
#include <array>
#include <exception>
#include <cstring>
#include <memory>

namespace UnitTest
{

template <typename Function, Function Address>
class ApiMock
{
private:
	template <typename ReturnValue, typename... Arguments>
	static std::function<ReturnValue(Arguments...)> GetCallback(ReturnValue (__stdcall*)(Arguments...));
	using Callback = decltype(GetCallback(Address));

	template <typename ReturnValue, typename... Arguments>
	class ApiIntercept
	{
	public:
		static ReturnValue __stdcall Intercept(Arguments... arguments)
		{
			if (currentSetup == nullptr)
				throw TestException{ "Intercept function called after lifetime of ApiMockSetup." };
			++currentSetup->callCount;
			return currentSetup->callback(arguments...);
		}
	};
	template <typename... Arguments>
	class ApiIntercept<void, Arguments...>
	{
	public:
		static void __stdcall Intercept(Arguments... arguments)
		{
			if (currentSetup == nullptr)
				throw TestException{ "Intercept function called after lifetime of ApiMockSetup." };
			++currentSetup->callCount;
			currentSetup->callback(arguments...);
		}
	};
	template <typename ReturnValue, typename... Arguments>
	static ApiIntercept<ReturnValue, Arguments...> GetApiIntercept(ReturnValue (__stdcall*)(Arguments...));
	using Intercept = decltype(GetApiIntercept(Address));

	#if defined(_M_X64)
	static const auto JumpInstructionSize = 2 + sizeof(Function) + 2;
	#else
	static const auto JumpInstructionSize = 1 + sizeof(std::ptrdiff_t);
	#endif
	using Instruction = unsigned char;
	using JumpInstruction = std::array<Instruction, JumpInstructionSize>;
	
public:
	class ApiMockSetup;

private:
	static ApiMockSetup* currentSetup;

public:
	class ApiMockSetup
	{
	public:
		ApiMockSetup(Callback callback)
			: callback(callback)
		{
			currentSetup = this;
			originalInstructions = ReadOriginalInstructions();
			WriteInstructions(CreateJumpInstruction());
		}
		~ApiMockSetup()
		{
			currentSetup = nullptr;
			auto safeToThrow = !std::uncaught_exception();
			try { WriteInstructions(originalInstructions); } catch(...) { if (safeToThrow) throw; }
			if (safeToThrow && callCount != expectedCallCount)
				throw TestException{ "Setup not matched." };
		}

		void Expect(int value)
		{
			expectedCallCount = value;
		}

	public:
		Callback callback;
		int callCount = 0;
		int expectedCallCount = 1;
		JumpInstruction originalInstructions;
	};

private:
	static JumpInstruction ReadOriginalInstructions()
	{
		JumpInstruction originalInstructions;
		auto result = ::ReadProcessMemory(
			::GetCurrentProcess(),
			reinterpret_cast<void*>(Address),
			originalInstructions.data(),
			originalInstructions.size(),
			nullptr);
		if (!result)
			throw TestException{ "ReadProcessMemory" };
		return originalInstructions;
	}

	static JumpInstruction CreateJumpInstruction()
	{
		JumpInstruction jumpToIntercept;
		#if defined(_M_X64)
		const Instruction move1 = 0x48;
		const Instruction move2 = 0xb8;
		const Instruction jump1 = 0xff;
		const Instruction jump2 = 0xe0;
		jumpToIntercept[0] = move1;
		jumpToIntercept[1] = move2;
		auto intercept = &Intercept::Intercept;
		std::memcpy(jumpToIntercept.data() + 2, reinterpret_cast<void*>(&intercept), sizeof(intercept));
		static_assert(sizeof(intercept) == 8, "Expected 64-bit pointer.");
		jumpToIntercept[2 + sizeof(intercept)] = jump1;
		jumpToIntercept[2 + sizeof(intercept) + 1] = jump2;
		#else
		const Instruction jumpRelative = 0xe9;
		jumpToIntercept[0] = jumpRelative;
		auto relativeOffset =
			reinterpret_cast<char*>(&Intercept::Intercept) -
			reinterpret_cast<char*>(Address) -
			jumpToIntercept.size();
		static_assert(sizeof(relativeOffset) == 4, "Expected 32-bit offset.");
		std::memcpy(jumpToIntercept.data() + 1, &relativeOffset, sizeof(relativeOffset));
		#endif
		return jumpToIntercept;
	}

	static void WriteInstructions(JumpInstruction instructions)
	{
		auto result = ::WriteProcessMemory(
			::GetCurrentProcess(),
			reinterpret_cast<void*>(Address),
			instructions.data(),
			instructions.size(),
			nullptr);
		if (!result)
			throw TestException{ "WriteProcessMemory" };
	}

public:
	static std::unique_ptr<ApiMockSetup> Setup(Callback value)
	{
		return std::unique_ptr<ApiMockSetup>{ new ApiMockSetup{ value } };
	}
};

template <typename Function, Function Address>
typename ApiMock<Function, Address>::ApiMockSetup* ApiMock<Function, Address>::currentSetup = nullptr;

}

#endif

