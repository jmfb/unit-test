#pragma once
#include <string>
#include <vector>
#include <functional>
#include "Any.h"
#include "ReturnValue.h"

namespace UnitTest
{

class CallData
{
public:
	CallData(unsigned long offset)
		: mOffset(offset), mExpectedCalls(1), mActualCalls(0)
	{
	}

	CallData(const CallData& rhs) = default;
	CallData(CallData&& rhs) = default;
	CallData& operator=(const CallData& rhs) = default;

	CallData& operator=(CallData&& rhs)
	{
		if (this != &rhs)
		{
			mOffset = std::move(rhs.mOffset);
			mArguments = std::move(rhs.mArguments);
			mExpectedCalls = std::move(rhs.mExpectedCalls);
			mActualCalls = std::move(rhs.mActualCalls);
			mThrowValue = std::move(rhs.mThrowValue);
			mReturnValue = std::move(rhs.mReturnValue);
			mCallback = std::move(rhs.mCallback);
		}
		return *this;
	}

	template <typename A>
	CallData& operator()(A argument)
	{
		mArguments.push_back(Any(argument));
		return *this;
	}

	class CallbackBase
	{
	public:
		virtual ~CallbackBase()
		{
		}
	};

	typedef std::shared_ptr<CallbackBase> CallbackPtr;

	template <typename T>
	class CallbackHolder : public CallbackBase
	{
	public:
		CallbackHolder(T callback)
			: mCallback(callback)
		{
		}

		T GetCallback() const
		{
			return mCallback;
		}

	private:
		T mCallback;
	};

	template <typename... TArgs>
	CallData& Callback(std::function<void(TArgs...)> callback)
	{
		typedef CallbackHolder<decltype(callback)> CallbackType;
		std::shared_ptr<CallbackType> pointer(new CallbackType(callback));
		mCallback = std::dynamic_pointer_cast<CallbackBase>(pointer);
		return *this;
	}

	template <typename... TArgs>
	void DoCallback(TArgs... args)
	{
		if (mCallback)
		{
			typedef CallbackHolder<std::function<void(TArgs...)>> CallbackType;
			auto callback = std::dynamic_pointer_cast<CallbackType>(mCallback);
			if (callback)
				callback->GetCallback()(args...);
		}
	}

	unsigned long mOffset;
	std::vector<Any> mArguments;
	unsigned long mExpectedCalls;
	unsigned long mActualCalls;
	Any mThrowValue;
	Any mReturnValue;
	CallbackPtr mCallback;
};

}

