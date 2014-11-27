#pragma once
#if defined(_WINDOWS_)
#include "Detour.h"
#include "TestException.h"
#include <functional>
#include <exception>
#include <utility>

namespace UnitTest
{

template <typename Function, Function Address>
class ApiMock
{
public:
	template <typename ReturnValue, typename... Arguments>
	static std::function<ReturnValue(Arguments...)> GetCallbackFunction(ReturnValue (__stdcall*)(Arguments...));
	using CallbackFunction = decltype(GetCallbackFunction(Address));

	ApiMock(CallbackFunction callback)
		: callback{ callback }, detour{ Address, &Intercept::Intercept }
	{
		currentMock = this;
	}
	~ApiMock()
	{
		currentMock = nullptr;
		if (callCount != expectedCallCount && !std::uncaught_exception())
			throw TestException{ "Setup not matched." };
	}

	ApiMock<Function, Address>& Expects(int value)
	{
		expectedCallCount = value;
		return *this;
	}

private:
	template <typename ReturnValue, typename... Arguments>
	ReturnValue CallbackAndReturn(Arguments&&... arguments)
	{
		++callCount;
		return callback(std::forward<Arguments>(arguments)...);
	}
	template <typename... Arguments>
	void Callback(Arguments&&... arguments)
	{
		++callCount;
		callback(std::forward<Arguments>(arguments)...);
	}
	
	template <typename ReturnValue, typename... Arguments>
	class Interceptor
	{
	public:
		static ReturnValue __stdcall Intercept(Arguments... arguments)
		{
			return currentMock->CallbackAndReturn<ReturnValue>(arguments...);
		}
	};
	template <typename... Arguments>
	class Interceptor<void, Arguments...>
	{
	public:
		static void __stdcall Intercept(Arguments... arguments)
		{
			currentMock->Callback(arguments...);
		}
	};
	template <typename ReturnValue, typename... Arguments>
	static Interceptor<ReturnValue, Arguments...> GetInterceptor(ReturnValue (__stdcall*)(Arguments...));
	using Intercept = decltype(GetInterceptor(Address));

private:
	static ApiMock<Function, Address>* currentMock;
	CallbackFunction callback;
	Detour detour;
	int callCount = 0;
	int expectedCallCount = 1;
};

template <typename Function, Function Address>
ApiMock<Function, Address>* ApiMock<Function, Address>::currentMock = nullptr;

}

#endif

