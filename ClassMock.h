#pragma once
#if defined(_WINDOWS_)
#include "Detour.h"
#include <exception>
#include <stdexcept>
#include <functional>
#include <utility>

template <typename MemberFunction, MemberFunction Address>
class ClassMock
{
public:
	template <typename ReturnValue, typename Class, typename... Arguments>
	static std::function<ReturnValue(Class*, Arguments...)> GetCallbackFunction(ReturnValue (Class::*)(Arguments...));
	template <typename ReturnValue, typename Class, typename... Arguments>
	static std::function<ReturnValue(const Class*, Arguments...)> GetCallbackFunction(ReturnValue (Class::*)(Arguments...) const);
	using CallbackFunction = decltype(GetCallbackFunction(Address));

	ClassMock(CallbackFunction callback)
		: callback{ callback }, detour{ Address, &Intercept::Intercept }
	{
		currentMock = this;
	}

	~ClassMock()
	{
		currentMock = nullptr;
		if (callCount != expectedCallCount && !std::uncaught_exception())
			throw std::runtime_error{ "Setup not matched." };
	}

	ClassMock<MemberFunction, Address>& Expects(int value)
	{
		expectedCallCount = value;
		return *this;
	}

private:
	template <typename ReturnValue, typename Class, typename... Arguments>
	ReturnValue CallbackAndReturn(Class* instance, Arguments&&... arguments)
	{
		++callCount;
		return callback(instance, std::forward<Arguments>(arguments)...);
	}
	template <typename Class, typename... Arguments>
	void Callback(Class* instance, Arguments&&... arguments)
	{
		++callCount;
		callback(instance, std::forward<Arguments>(arguments)...);
	}

	template <typename ReturnValue, typename Class, typename... Arguments>
	class Interceptor
	{
	public:
		ReturnValue Intercept(Arguments... arguments)
		{
			return currentMock->CallbackAndReturn<ReturnValue>(reinterpret_cast<Class*>(this), arguments...);
		}
	};
	template <typename Class, typename... Arguments>
	class Interceptor<void, Class, Arguments...>
	{
	public:
		void Intercept(Arguments... arguments)
		{
			currentMock->Callback(reinterpret_cast<Class*>(this), arguments...);
		}
	};
	template <typename ReturnValue, typename Class, typename... Arguments>
	static Interceptor<ReturnValue, Class, Arguments...> GetInterceptor(ReturnValue (Class::*)(Arguments...));
	template <typename ReturnValue, typename Class, typename... Arguments>
	static Interceptor<ReturnValue, const Class, Arguments...> GetInterceptor(ReturnValue(Class::*)(Arguments...) const);
	using Intercept = decltype(GetInterceptor(Address));

private:
	static ClassMock<MemberFunction, Address>* currentMock;
	CallbackFunction callback;
	Detour detour;
	int callCount = 0;
	int expectedCallCount = 1;
};

template <typename MemberFunction, MemberFunction Address>
ClassMock<MemberFunction, Address>* ClassMock<MemberFunction, Address>::currentMock = nullptr;

#endif

