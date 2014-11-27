#pragma once
#include <vector>
#include <list>
#include <map>
#include <typeinfo>
#include <exception>
#include <sstream>
#include <type_traits>
#include <tuple>
#include "Const.h"
#include "ForcedCast.h"
#include "VirtualTable.h"
#include "OffsetHelper.h"
#include "ReturnValue.h"
#include "Any.h"
#include "CallData.h"
#include "SetupData.h"
#include "ArgumentList.h"
#include "FunctionHelper.h"
#include "PackParameters.h"
#include "TestException.h"
#include "TypeName.h"

namespace UnitTest
{

template <typename T>
class Mock
{
public:
	Mock();
	Mock(const Mock<T>& rhs) = delete;
	Mock(Mock<T>&& rhs) = delete;
	Mock& operator=(const Mock<T>& rhs) = delete;
	Mock& operator=(Mock<T>&& rhs) = delete;

	std::shared_ptr<T> GetObject()
	{
		return std::shared_ptr<T>(mTable.GetInterfacePtr<T>(), [](T*){});
	}

	template <typename TFunction, typename... TParams>
	auto Setup(TFunction function, TParams... params) ->
		SetupData<decltype(GetFunctionResult(function)), decltype(GetFunctionCallback(function))>;

	//NOTE: Could create a SetupDestructor function, but I don't see the mock usage.

	void Verify()
	{
		bool failed = false;
		std::ostringstream out;
		out << "Mock<" << TypeName<T>::Get() << ">::Verify: the following setups were not matched." << std::endl;
		for (auto mapIter = mCallMap.begin(), mapEnd = mCallMap.end(); mapIter != mapEnd; ++mapIter)
		{
			for (auto listIter = mapIter->second.begin(), listEnd = mapIter->second.end(); listIter != listEnd; ++listIter)
			{
				if (listIter->mExpectedCalls != listIter->mActualCalls)
				{
					failed = true;
					out << "Offset " << mapIter->first << " with signature " << mCallSignature[mapIter->first]
						<< " expected " << listIter->mExpectedCalls << ", actual " << listIter->mActualCalls << std::endl;
					if (!listIter->mArguments.empty())
						out << "Match arguments: " << FormatArgumentList(listIter->mArguments) << std::endl;
				}
			}
		}
		if (failed)
			throw TestException(out.str());
	}

	void NotImplemented(unsigned long index)
	{
		std::ostringstream out;
		out << "Mock<" << TypeName<T>::Get() << "> had no matching setup for function at offset " << index << ".";
		throw TestException(out.str());
	}

	template <typename TResult, typename... TArgs>
	void Invoke(unsigned long index, ReturnValue<TResult>& returnValue, TArgs... args)
	{
		auto mapIter = mCallMap.find(index);
		if (mapIter == mCallMap.end())
			throw TestException("Mock<T>::Invoke: Invalid callback index.");

		ArgumentList arguments;
		BuildArgumentList<TArgs...>::Build(arguments, args...);

		for (auto listIter = mapIter->second.begin(), last = mapIter->second.end(); listIter != last; ++listIter)
		{
			if (listIter->mArguments == arguments)
			{
				++listIter->mActualCalls;
				listIter->DoCallback<TArgs...>(args...);
				listIter->mThrowValue.Throw();
				returnValue.Set(listIter->mReturnValue);
				return;
			}
		}

		std::ostringstream out;
		out << "Mock<" << TypeName<T>::Get() << ">::Invoke: no matching setup for function at offset "
			<< index << " with signature " << mCallSignature[index] << " and arguments:" << std::endl
			<< FormatArgumentList(arguments);
		throw TestException(out.str());
	}

private:
	static std::string FormatArgumentList(const std::vector<Any>& arguments)
	{
		std::ostringstream out;
		for (auto begin = arguments.begin(), iter = begin, end = arguments.end(); iter != end; ++iter)
		{
			if (iter != begin)
				out << ", ";
			out << iter->ToString();
		}
		return out.str();
	}

private:
	VirtualTable mTable;
	std::map<unsigned long, std::list<CallData>> mCallMap;
	std::map<unsigned long, std::string> mCallSignature;
};

template <typename T, unsigned long I>
inline void NotImplementedPlaceholder(void* pThis)
{
	VirtualTable::GetObjectFromThis<Mock<T>>(pThis)->NotImplemented(I);
}

typedef void (*NotImplementedFunction)(void*);

template <unsigned long I>
class FindNotImplementedFunction
{
public:
	template <typename T>
	static NotImplementedFunction Find(unsigned long offset, Mock<T>* mock)
	{
		if (offset == I)
			return &NotImplementedPlaceholder<T, I>;
		return FindNotImplementedFunction<I + 1>::Find(offset, mock);
	}
};

template <>
class FindNotImplementedFunction<MAX_VIRTUAL_FUNCTIONS>
{
public:
	template <typename T>
	static NotImplementedFunction Find(unsigned long offset, Mock<T>* mock)
	{
		throw TestException("FindNotImplementedFunction: Exceeded compiler template expansion limit.");
	}
};

template <typename T>
Mock<T>::Mock()
{
	mTable.SetObject(this);
	for (unsigned long i = 0; i < MAX_VIRTUAL_FUNCTIONS; ++i)
		mTable.InstallFunction(i, FindNotImplementedFunction<0>::Find(i, this));
}

template <unsigned long IOffset, typename TResult, typename TClass, typename... TArgsTuple>
class InvokeHelper
{
};

template <unsigned long IOffset, typename TResult, typename TClass, typename... TArgs>
class InvokeHelper<IOffset, TResult, TClass, std::tuple<TArgs...>>
{
public:
	static TResult Placeholder(TClass* pThis, TArgs... args)
	{
		ReturnValue<TResult> returnValue;
		VirtualTable::GetObjectFromThis<Mock<TClass>>(pThis)->
			template Invoke<TResult, TArgs...>(IOffset, returnValue, args...);
		return returnValue.Get();
	}
};

template <unsigned long IOffset, typename TClass, typename... TArgs>
class InvokeHelper<IOffset, void, TClass, std::tuple<TArgs...>>
{
public:
	static void Placeholder(TClass* pThis, TArgs... args)
	{
		ReturnValue<void> returnValue;
		VirtualTable::GetObjectFromThis<Mock<TClass>>(pThis)->
			template Invoke<void, TArgs...>(IOffset, returnValue, args...);
	}
};

template <unsigned long IOffset>
class FindInvokeHelper
{
public:
	template <typename TFunction>
	static void* Find(unsigned long offset, TFunction function)
	{
		typedef decltype(GetFunctionHelper(function)) FunctionHelper;
		typedef typename FunctionHelper::Result Result;
		typedef typename FunctionHelper::Class Class;
		typedef typename FunctionHelper::ArgsTuple ArgsTuple;
		typedef InvokeHelper<IOffset, Result, Class, ArgsTuple> Helper;
		if (offset == IOffset)
			return ForcedCast<void*>(&Helper::Placeholder);
		return FindInvokeHelper<IOffset + 1>::Find(offset, function);
	}
};

template <>
class FindInvokeHelper<MAX_VIRTUAL_FUNCTIONS>
{
public:
	template <typename TFunction>
	static void* Find(unsigned long offset, TFunction function)
	{
		throw TestException("FindInvokeHelper::Find: Exceeded compiler template expansion limit.");
	}
};

template <typename... TArgsTupleAndParams>
class MockSetupCheckParameters
{
};

template <typename TArg, typename... TArgs, typename TParam, typename... TParams>
class MockSetupCheckParameters<std::tuple<TArg, TArgs...>, TParam, TParams...>
	: public std::integral_constant<bool,
		(	std::is_same<Any::MatchEnum, TParam>::value
		||	std::is_convertible<TParam, TArg>::value)
		&&	MockSetupCheckParameters<std::tuple<TArgs...>, TParams...>::value>
{
};

template <>
class MockSetupCheckParameters<std::tuple<>>
	: public std::integral_constant<bool, true>
{
};

template <typename T>
template <typename TFunction, typename... TParams>
inline auto Mock<T>::Setup(TFunction function, TParams... params) ->
	SetupData<decltype(GetFunctionResult(function)), decltype(GetFunctionCallback(function))>
{
	static_assert(std::is_member_function_pointer<TFunction>::value,
		"First argument to Mock<T>::Setup must be a member function pointer.");

	typedef decltype(GetFunctionHelper(function)) FunctionHelper;
	typedef typename FunctionHelper::Result Result;
	typedef typename FunctionHelper::Class Class;
	typedef typename FunctionHelper::ArgsTuple ArgsTuple;
	typedef decltype(GetFunctionCallback(function)) CallbackFunctionType;
	static_assert(std::is_base_of<Class, T>::value,
		"Function must be a member of T or a base.");

	constexpr unsigned long ArgumentCount = std::tuple_size<ArgsTuple>::value;
	constexpr unsigned long ParametersCount = std::tuple_size<std::tuple<TParams...>>::value;
	static_assert(ArgumentCount == ParametersCount,
		"Incorrect number of expected parameters for given function.");

	static_assert(MockSetupCheckParameters<ArgsTuple, TParams...>::value,
		"Parameters are not convertible to the function arguments.");

	OffsetHelper offsetHelper;
	unsigned long offset = offsetHelper.GetOffset(function);

	mTable.InstallFunction(offset, FindInvokeHelper<0>::Find(offset, function));

	CallData callData(offset);
	PackParameters<ArgsTuple, 0, TParams...>::Pack(callData, params...);

	mCallSignature[offset] = TypeName<TFunction>::Get();

	auto& callList = mCallMap[offset];
	if (ArgumentCount == 0)
		callList.clear();
	callList.push_back(callData);
	return SetupData<Result, CallbackFunctionType>(callList.back());
}

}

