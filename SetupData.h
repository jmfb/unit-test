#pragma once
#include "CallData.h"

namespace UnitTest
{

template <typename R, typename... TArgs>
class SetupData
{
};

template <typename R, typename... TArgs>
class SetupData<R, std::function<void(TArgs...)>>
{
public:
	typedef std::function<void(TArgs...)> CallbackFunctionType;
	typedef SetupData<R, CallbackFunctionType> ThisType;

	SetupData(CallData& callData)
		: mCallData(callData)
	{
	}

	ThisType& Returns(R returnValue)
	{
		mCallData.mReturnValue.Set<R>(returnValue);
		return *this;
	}
	ThisType& Expects(unsigned long count)
	{
		mCallData.mExpectedCalls = count;
		return *this;
	}
	template <typename E>
	ThisType& Throws(E exception)
	{
		mCallData.mThrowValue = exception;
		return *this;
	}
	ThisType& Callback(CallbackFunctionType callback)
	{
		mCallData.Callback(callback);
		return *this;
	}

private:
	CallData& mCallData;
};

template <typename... TArgs>
class SetupData<void, std::function<void(TArgs...)>>
{
public:
	typedef std::function<void(TArgs...)> CallbackFunctionType;
	typedef SetupData<void, CallbackFunctionType> ThisType;

	SetupData(CallData& callData)
		: mCallData(callData)
	{
	}

	ThisType& Expects(unsigned long count)
	{
		mCallData.mExpectedCalls = count;
		return *this;
	}
	template <typename E>
	ThisType& Throws(E exception)
	{
		mCallData.mThrowValue = exception;
		return *this;
	}
	ThisType& Callback(CallbackFunctionType callback)
	{
		mCallData.Callback(callback);
		return *this;
	}

private:
	CallData& mCallData;
};

}

