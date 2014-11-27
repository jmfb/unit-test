#pragma once

namespace UnitTest
{

template <typename TOutput, typename TInput>
inline TOutput ForcedCast(TInput input)
{
	union ForcedCastUnion
	{
	public:
		ForcedCastUnion(TInput input)
			: mInput(input)
		{
		}

		TOutput mOutput;
		TInput mInput;
	};
	static_assert(sizeof(TInput) == sizeof(TOutput), "Cannot force a cast between values of different size.");
	return ForcedCastUnion(input).mOutput;
}

}

