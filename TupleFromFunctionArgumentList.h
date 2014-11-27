#pragma once
#include <functional>
#include <tuple>
#include <type_traits>

namespace UnitTest
{

// TupleFromFunctionArgumentList is a helper class to deduce a tuple that contains the
// root types (pointer attribute removed) from a function object type argument list.
// The tuple typedef is called Type.
//
// Example:
//	typedef TupleFromFunctionArgumentList<std::function<void(IFoo*,IBar*)>>::Type TupleType;
//	//is equivalent to
//	typedef std::tuple<IFoo, IBar> TupleType;
template <typename... TArgs>
class TupleFromFunctionArgumentList
{
	//nothing (should only ever use function specialization)
};

template <typename... TArgs>
class TupleFromFunctionArgumentList<std::function<void(TArgs...)>>
{
public:
	typedef std::tuple<typename std::remove_pointer<TArgs>::type...> Type;
};

}

