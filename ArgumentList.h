#pragma once
#include <vector>
#include "Any.h"

namespace UnitTest
{

class ArgumentList : public std::vector<Any>
{
public:
	ArgumentList() = default;
	ArgumentList(const ArgumentList& rhs) = default;
	ArgumentList(ArgumentList&& rhs) = default;
	ArgumentList& operator=(const ArgumentList& rhs) = default;

	ArgumentList& operator=(ArgumentList&& rhs)
	{
		if (this != &rhs)
			std::vector<Any>::operator=(rhs);
		return *this;
	}

	ArgumentList(const Any& value)
	{
		push_back(value);
	}
	ArgumentList& operator()(const Any& value)
	{
		push_back(value);
		return *this;
	}
};

template <typename... TArgs>
class BuildArgumentList
{
};

template <typename TArg, typename... TArgs>
class BuildArgumentList<TArg, TArgs...>
{
public:
	static void Build(ArgumentList& arguments, TArg arg, TArgs... args)
	{
		arguments(arg);
		BuildArgumentList<TArgs...>::Build(arguments, args...);
	}
};

template <>
class BuildArgumentList<>
{
public:
	static void Build(ArgumentList& arguments)
	{
	}
};

}

