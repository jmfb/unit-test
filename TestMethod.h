#pragma once
#include "ITestMethod.h"
#include "TestMethodRegister.h"

namespace UnitTest
{

template <typename T, typename TMethod>
class TestMethod : public ITestMethod
{
public:
	static TestMethodRegister<T, TMethod> mAutoRegister;

	TestMethod(T& object)
		: mObject(object)
	{
	}
	virtual ~TestMethod()
	{
//The following "seemingly" unused variable reference is required for automatic registration.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
		mAutoRegister;
	}
#pragma GCC diagnostic pop

	virtual const char* GetTestMethodName() const
	{
		return TMethod::GetTestMethodNameStatic();
	}
	virtual const char* GetTestMethodLocation() const
	{
		return TMethod::GetTestMethodLocationStatic();
	}
	virtual void Execute() const
	{
		TMethod::ExecuteStatic(mObject);
	}

private:
	T& mObject;
};

template <typename T, typename TMethod>
TestMethodRegister<T, TMethod> TestMethod<T, TMethod>::mAutoRegister;

#define TEST_METHOD_STRINGIZE_LINE_NUMBER_CORE(lineNumber) #lineNumber
#define TEST_METHOD_STRINGIZE_LINE_NUMBER(lineNumber) TEST_METHOD_STRINGIZE_LINE_NUMBER_CORE(lineNumber)

#define TEST_METHOD(Name) \
	class TestMethod##Name : public UnitTest::TestMethod<DerivedTestClass, TestMethod##Name> \
	{ \
	public: \
		TestMethod##Name(DerivedTestClass& object) \
			: UnitTest::TestMethod<DerivedTestClass, TestMethod##Name>(object) \
		{ \
		} \
		static const char* GetTestMethodNameStatic() \
		{ \
			return #Name; \
		} \
		static const char* GetTestMethodLocationStatic() \
		{ \
			return __FILE__ ":" TEST_METHOD_STRINGIZE_LINE_NUMBER(__LINE__) ":0"; \
		} \
		static void ExecuteStatic(DerivedTestClass& object) \
		{ \
			object.Name(); \
		} \
	}; \
	void Name()

}

