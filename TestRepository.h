#pragma once
#include <vector>
#include <memory>
#include "TestClassFactory.h"

namespace UnitTest
{

class TestRepository
{
private:
	TestRepository()
	{
	}
	~TestRepository()
	{
	}

public:
	static TestRepository& GetInstance()
	{
		static TestRepository instance;
		return instance;
	}

	template <typename T>
	void Register()
	{
		mFactories.push_back(&TestClassFactory<T>::GetInstance());
	}

	unsigned long GetCount() const
	{
		return mFactories.size();
	}

	ITestClassFactory& Get(unsigned long index) const
	{
		return *mFactories[index];
	}

private:
	std::vector<ITestClassFactory*> mFactories;
};

}

