#pragma once
#if defined(_WINDOWS_)
#include "Detour.h"
#include "TestException.h"
#include <functional>
#include <exception>

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
			: callback{ callback }, detour{ Address, &Intercept::Intercept }
		{
			currentSetup = this;
		}
		~ApiMockSetup()
		{
			currentSetup = nullptr;
			if (callCount != expectedCallCount && !std::uncaught_exception())
				throw TestException{ "Setup not matched." };
		}

		void Expect(int value)
		{
			expectedCallCount = value;
		}

	public:
		Callback callback;
		Detour detour;
		int callCount = 0;
		int expectedCallCount = 1;
	};

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

