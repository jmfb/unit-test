#pragma once
#include <tuple>
#include "Any.h"
#include "CallData.h"

namespace UnitTest
{

template <typename TArgsTuple, unsigned long IArgIndex, typename... TParams>
class PackParameters
{
};

template <typename TArgsTuple, unsigned long IArgIndex, typename TParam, typename... TParams>
class PackParameters<TArgsTuple, IArgIndex, TParam, TParams...>
{
public:
	static void Pack(CallData& callData, TParam param, TParams... params)
	{
		typedef typename std::tuple_element<IArgIndex, TArgsTuple>::type Arg;
		callData(any_cast<Arg>(param));
		PackParameters<TArgsTuple, IArgIndex + 1, TParams...>::Pack(callData, params...);
	}
};

template <typename TArgsTuple, unsigned long IArgIndex>
class PackParameters<TArgsTuple, IArgIndex>
{
public:
	static void Pack(CallData& callData)
	{
	}
};

}

