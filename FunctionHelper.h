#pragma once
#include <tuple>
#include <functional>

namespace UnitTest
{

template <typename TResult, typename TClass, typename... TArgs>
class FunctionHelper
{
public:
	typedef TResult Result;
	typedef TClass Class;
	typedef std::tuple<TArgs...> ArgsTuple;
};

template <typename TResult, typename TClass, typename... TArgs>
FunctionHelper<TResult, TClass, TArgs...> GetFunctionHelper(TResult (TClass::*)(TArgs...));

template <typename TResult, typename TClass, typename... TArgs>
FunctionHelper<TResult, TClass, TArgs...> GetFunctionHelper(TResult (TClass::*)(TArgs...) const);

template <typename TResult, typename TClass, typename... TArgs>
FunctionHelper<TResult, TClass, TArgs...> GetFunctionHelper(TResult (TClass::*)(TArgs...) volatile);

template <typename TResult, typename TClass, typename... TArgs>
FunctionHelper<TResult, TClass, TArgs...> GetFunctionHelper(TResult (TClass::*)(TArgs...) const volatile);

template <typename TResult, typename TClass, typename... TArgs>
TResult GetFunctionResultFromHelper(FunctionHelper<TResult, TClass, TArgs...>);

template <typename TResult, typename TClass, typename... TArgs>
std::function<void(TArgs...)> GetFunctionCallbackFromHelper(FunctionHelper<TResult, TClass, TArgs...>);

template <typename TFunction>
auto GetFunctionResult(TFunction function) ->
	decltype(GetFunctionResultFromHelper(GetFunctionHelper(function)));

template <typename TFunction>
auto GetFunctionCallback(TFunction function) ->
	decltype(GetFunctionCallbackFromHelper(GetFunctionHelper(function)));

}

