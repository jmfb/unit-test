#pragma once
#include "ITestMethodFactory.h"

namespace UnitTest
{

template <typename T, typename TMethod>
class TestMethodFactory : public ITestMethodFactory
{
private:
	TestMethodFactory()
	{
	}
	~TestMethodFactory()
	{
	}

public:
	static TestMethodFactory<T, TMethod>& GetInstance()
	{
		static TestMethodFactory<T, TMethod> instance;
		return instance;
	}

	virtual const char* GetTestMethodName() const
	{
		return TMethod::GetTestMethodNameStatic();
	}
	virtual const char* GetTestMethodLocation() const
	{
		return TMethod::GetTestMethodLocationStatic();
	}
	virtual std::shared_ptr<ITestMethod> CreateInstance(ITestClass* object) const
	{
		return std::shared_ptr<ITestMethod>(new TMethod(*dynamic_cast<T*>(object)));
	}
};

}

