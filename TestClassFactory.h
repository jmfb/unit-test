#pragma once
#include <vector>
#include "ITestClassFactory.h"
#include "ITestMethodFactory.h"

namespace UnitTest
{

template <typename T>
class TestClassFactory : public ITestClassFactory
{
private:
	TestClassFactory()
	{
	}
	~TestClassFactory()
	{
	}

public:
	static TestClassFactory<T>& GetInstance()
	{
		static TestClassFactory<T> instance;
		return instance;
	}

	void Register(ITestMethodFactory* factory)
	{
		mFactories.push_back(factory);
	}

	virtual const char* GetTestClassName() const
	{
		return T::GetTestClassNameStatic();
	}
	virtual void InitializeTests()
	{
		T::InitializeTests();
	}
	virtual std::shared_ptr<ITestClass> CreateInstance() const
	{
		return std::shared_ptr<ITestClass>(new T());
	}
	virtual void TerminateTests()
	{
		T::TerminateTests();
	}

	unsigned long GetCount() const
	{
		return mFactories.size();
	}
	ITestMethodFactory& Get(unsigned long index) const
	{
		return *mFactories[index];
	}

private:
	std::vector<ITestMethodFactory*> mFactories;
};

}

