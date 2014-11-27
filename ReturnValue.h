#pragma once
#include "Any.h"

namespace UnitTest
{

template <typename R>
class ReturnValue
{
public:
	ReturnValue()
		: mValue(R())
	{
	}

	void Set(const Any& value)
	{
		if (value.IsNull())
			throw TestException("Return value not set for mocked function.");
		mValue = value.GetValue<R>();
	}
	R Get()
	{
		return mValue;
	}
private:
	R mValue;
};

template <typename R>
class ReturnValue<R&>
{
public:
	ReturnValue()
		: mValue(nullptr)
	{
	}

	void Set(const Any& value)
	{
		if (value.IsNull())
			throw TestException("Return value not set for mocked function.");
		mValue = &value.GetValue<R&>();
	}
	R& Get()
	{
		return *mValue;
	}
private:
	R* mValue;
};

template <typename R>
class ReturnValue<const R&>
{
public:
	ReturnValue()
		: mValue(nullptr)
	{
	}

	void Set(const Any& value)
	{
		if (value.IsNull())
			throw TestException("Return value not set for mocked function.");
		mValue = &value.GetValue<const R&>();
	}
	const R& Get()
	{
		return *mValue;
	}
private:
	const R* mValue;
};

template <>
class ReturnValue<void>
{
public:
	void Set(const Any& value)
	{
	}
};

}

