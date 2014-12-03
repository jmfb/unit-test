#pragma once
#include <string>
#include <memory>
#include <functional>
#include <exception>
#include "TestException.h"
#include "TypeName.h"

namespace UnitTest
{

class TestAssert
{
public:
	static void IsTrue(bool condition, const std::string& message = "")
	{
		if (!condition)
			TestException::Raise("Assert.IsTrue", message);
	}
	static void IsFalse(bool condition, const std::string& message = "")
	{
		if (condition)
			TestException::Raise("Assert.IsFalse", message);
	}
	static void IsNull(const void* value, const std::string& message = "")
	{
		if (value != nullptr)
			TestException::Raise("Assert.IsNull", message);
	}
	static void IsNotNull(const void* value, const std::string& message = "")
	{
		if (value == nullptr)
			TestException::Raise("Assert.IsNotNull", message);
	}
	template <typename T1, typename T2>
	static void AreEqual(const T1& expected, const T2& actual, const std::string& message = "")
	{
		if (!(expected == actual))
			TestException::Raise("Assert.AreEqual", "expected", expected, "actual", actual, message);
	}
	template <typename T1, typename T2>
	static void AreNotEqual(const T1& notExpected, const T2& actual, const std::string& message = "")
	{
		if (notExpected == actual)
			TestException::Raise("Assert.AreNotEqual", "notExpected", notExpected, "actual", actual, message);
	}
	static void Fail(const std::string& message = "")
	{
		TestException::Raise("Assert.Fail", message);
	}
	static void Inconclusive(const std::string& message = "")
	{
		TestException::Raise("Assert.Inconclusive", message);
	}
	static void Throws(std::function<void()> action, const std::string& expectedMessage, const std::string& message = "")
	{
		try
		{
			action();
		}
		catch (const TestException& error)
		{
			//re-throw test exceptions (the action has already failed for another testing reason - do not hide that exception)
			throw;
		}
		catch (const std::exception& error)
		{
			if (error.what() == expectedMessage)
				return;
			TestException::Raise("Assert.Throws", "expected", expectedMessage, "actual", error.what(), message);
		}
		catch (...)
		{
			TestException::Raise("Assert.Throws", "expected", expectedMessage, "actual(...)", "unhandled exception", message);
		}
		TestException::Raise("Assert.Throws", "expected", expectedMessage, "actual", "No exception was thrown.", message);
	}
	template <typename T>
	static void ThrowsType(std::function<void()> action, const std::string& expectedMessage, const std::string& message = "")
	{
		try
		{
			action();
		}
		catch (const TestException& error)
		{
			//re-throw test exceptions (the action has already failed for another testing reason - do not hide that exception)
			throw;
		}
		catch (const T& error)
		{
			if (error.what() == expectedMessage)
				return;
			TestException::Raise("Assert.ThrowsType", "expected", expectedMessage, "actual", error.what(), message);
		}
		catch (const std::exception& error)
		{
			TestException::Raise("Assert.ThrowsType", "expected", expectedMessage, "actual(wrong type)", error.what(), message);
		}
		catch (...)
		{
			TestException::Raise("Assert.ThrowsType", "expected", expectedMessage, "actual(...)", "unhandled exception", message);
		}
		TestException::Raise("Assert.ThrowsType", "expected", expectedMessage, "actual", "No exception was thrown.", message);
	}
	template <typename T>
	static void ThrowsType(std::function<void()> action, std::function<void(const T&)> callback, const std::string& message = "")
	{
		try
		{
			action();
			TestException::Raise("Assert.ThrowsType", "expected", TypeName<T>::Get(), "actual", "No exception was thrown.", message);
		}
		catch (const TestException& exception)
		{
			throw;
		}
		catch (const T& exception)
		{
			callback(exception);
		}
	}
	template <typename T>
	static void AreSame(const T& expected, const T& actual, const std::string& message = "")
	{
		if (std::addressof(expected) != std::addressof(actual))
			TestException::Raise(
				"Assert.AreSame",
				"expected",
				std::addressof(expected),
				"actual",
				std::addressof(actual),
				message);
	}
	template <typename T>
	static void AreNotSame(const T& notExpected, const T& actual, const std::string& message = "")
	{
		if (std::addressof(notExpected) == std::addressof(actual))
			TestException::Raise(
				"Assert.AreNotSame",
				"notExpected",
				std::addressof(notExpected),
				"actual",
				std::addressof(actual),
				message);
	}
	//TODO: StringAssert helpers
	//TODO: CollectionAssert helpers
};

static const UnitTest::TestAssert Assert = UnitTest::TestAssert();

}

