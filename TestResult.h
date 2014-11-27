#pragma once
#include <string>

namespace UnitTest
{

class TestResult
{
public:
	TestResult()
		: mTestClass(""), mTestMethod(""), mPassed(false)
	{
	}

	TestResult(
		const char* testClass,
		const char* testMethod,
		bool passed = false,
		const std::string& description = "")
		: mTestClass(testClass),
		mTestMethod(testMethod),
		mPassed(passed),
		mDescription(description)
	{
	}

	TestResult(const TestResult& rhs)
		: mTestClass(rhs.mTestClass),
		mTestMethod(rhs.mTestMethod),
		mPassed(rhs.mPassed),
		mDescription(rhs.mDescription)
	{
	}

	~TestResult()
	{
	}

	TestResult& operator=(const TestResult& rhs)
	{
		if (this != &rhs)
		{
			mTestClass = rhs.mTestClass;
			mTestMethod = rhs.mTestMethod;
			mPassed = rhs.mPassed;
			mDescription = rhs.mDescription;
		}
		return *this;
	}

	const char* GetTestClass() const
	{
		return mTestClass;
	}
	const char* GetTestMethod() const
	{
		return mTestMethod;
	}
	bool GetPassed() const
	{
		return mPassed;
	}
	const std::string& GetDescription() const
	{
		return mDescription;
	}

	void Pass()
	{
		mPassed = true;
		mDescription.erase();
	}
	void Fail(const std::string& description)
	{
		mPassed = false;
		mDescription = description;
	}

private:
	const char* mTestClass;
	const char* mTestMethod;
	bool mPassed;
	std::string mDescription;
};

}

