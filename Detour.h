#pragma once
#if defined(_WINDOWS_)
#include <exception>
#include <cstdint>
#include <array>
#include <cstring>
#include <stdexcept>

namespace UnitTest
{

class Detour
{
public:
	template <typename OriginalFunction, typename InterceptFunction>
	Detour(OriginalFunction originalFunction, InterceptFunction interceptFunction)
	{
		overwriteAddress = PointerToFunction(originalFunction);
		for (auto jumpIndex = 0;; ++jumpIndex)
		{
			originalInstructions = ReadInstructionsAt(overwriteAddress);
			if (CanSafelyOverwriteInstructions(originalInstructions))
				break;
			if (jumpIndex > 10)
				throw std::runtime_error{ "Exceeded jump table linking before installing detour." };
			overwriteAddress = GetRelativeJumpTargetAddress(overwriteAddress, originalInstructions);
		}
		auto jumpInstructions = CreateJumpInstruction(
			PointerToFunction(originalFunction),
			PointerToFunction(interceptFunction));
		OverwriteInstructionsAt(overwriteAddress, jumpInstructions);
	}

	~Detour()
	{
		auto safeToThrow = !std::uncaught_exception();
		try
		{
			OverwriteInstructionsAt(overwriteAddress, originalInstructions);
		}
		catch (...)
		{
			if (safeToThrow)
				throw;
		}
	}

private:
	static const auto RelativeJumpInstructionSize = 1 + sizeof(std::int32_t);
#if defined(_M_X64)
	static const auto JumpInstructionSize = 2 + sizeof(void*) + 2;
#else
	static const auto JumpInstructionSize = RelativeJumpInstructionSize;
#endif
	using Instruction = unsigned char;
	using JumpInstruction = std::array<Instruction, JumpInstructionSize>;
	static const Instruction RelativeJump = 0xe9;

	JumpInstruction CreateJumpInstruction(void* originalAddress, void* interceptAddress)
	{
		JumpInstruction jumpToIntercept;
#if defined(_M_X64)
		const Instruction move1 = 0x48;
		const Instruction move2 = 0xb8;
		const Instruction jump1 = 0xff;
		const Instruction jump2 = 0xe0;
		jumpToIntercept[0] = move1;
		jumpToIntercept[1] = move2;
		std::memcpy(jumpToIntercept.data() + 2, &interceptAddress, sizeof(interceptAddress));
		static_assert(sizeof(void*) == 8, "Expected 64-bit pointer.");
		jumpToIntercept[2 + sizeof(void*)] = jump1;
		jumpToIntercept[2 + sizeof(void*) + 1] = jump2;
#else
		auto relativeOffset =
			reinterpret_cast<char*>(interceptAddress) -
			reinterpret_cast<char*>(originalAddress) -
			JumpInstructionSize;
		jumpToIntercept[0] = RelativeJump;
		std::memcpy(jumpToIntercept.data() + 1, &relativeOffset, sizeof(relativeOffset));
#endif
		return jumpToIntercept;
	}

	JumpInstruction ReadInstructionsAt(void* address)
	{
		JumpInstruction instructions;
		auto result = ::ReadProcessMemory(
			::GetCurrentProcess(),
			address,
			instructions.data(),
			instructions.size(),
			nullptr);
		if (!result)
			throw std::runtime_error{ "ReadProcessMemory" };
		return instructions;
	}

	bool IsRelativeJumpInstruction(JumpInstruction instructions)
	{
		return instructions[0] == RelativeJump;
	}

	void* GetRelativeJumpTargetAddress(void* baseAddress, JumpInstruction instructions)
	{
		std::int32_t relativeOffset = 0;
		std::memcpy(&relativeOffset, instructions.data() + 1, sizeof(relativeOffset));
		return reinterpret_cast<char*>(baseAddress) + relativeOffset + RelativeJumpInstructionSize;
	}

	bool CanSafelyOverwriteInstructions(JumpInstruction instructions)
	{
		return !IsRelativeJumpInstruction(instructions) ||
			JumpInstructionSize <= RelativeJumpInstructionSize;
	}

	void OverwriteInstructionsAt(void* address, JumpInstruction instructions)
	{
		auto result = ::WriteProcessMemory(
			::GetCurrentProcess(),
			address,
			instructions.data(),
			instructions.size(),
			nullptr);
		if (!result)
			throw std::runtime_error{ "WriteProcessMemory" };
	}

	template <typename Function>
	void* PointerToFunction(Function function)
	{
		return *reinterpret_cast<void**>(&function);
	}

private:
	void* overwriteAddress = nullptr;
	JumpInstruction originalInstructions;
};

}

#endif

