#pragma once
#include "ITestClass.h"
#include "TestClassRegister.h"

namespace UnitTest
{

template <typename T, typename TName>
class TestClass : public ITestClass
{
public:
	typedef T DerivedTestClass;

	static TestClassRegister<T> mAutoRegister;

	TestClass()
	{
	}
	virtual ~TestClass()
	{
//The following "seemingly" unused variable reference is required for automatic registration.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
		mAutoRegister;
	}
#pragma GCC diagnostic pop

	static void InitializeTests()
	{
		//overridable
	}
	static void TerminateTests()
	{
		//overridable
	}

	static const char* GetTestClassNameStatic()
	{
		return TName::GetTestClassName();
	}
	virtual const char* GetTestClassName() const
	{
		return GetTestClassNameStatic();
	}

	virtual void BeginTest()
	{
		//overridable
	}
	virtual void EndTest()
	{
		//overridable
	}
};

template <typename T, typename TName>
TestClassRegister<T> TestClass<T, TName>::mAutoRegister;

#define TEST_CLASS(Name) \
	class TestClassName##Name \
	{ \
	public: \
		static const char* GetTestClassName() \
		{ \
			return #Name; \
		} \
	}; \
	class Name : public UnitTest::TestClass<Name, TestClassName##Name>

}

