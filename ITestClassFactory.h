#pragma once
#include <memory>
#include "ITestClass.h"
#include "ITestMethodFactory.h"

namespace UnitTest
{

class ITestClassFactory
{
public:
	virtual const char* GetTestClassName() const = 0;
	virtual void InitializeTests() = 0;
	virtual std::shared_ptr<ITestClass> CreateInstance() const = 0;
	virtual void TerminateTests() = 0;
	virtual unsigned long GetCount() const = 0;
	virtual ITestMethodFactory& Get(unsigned long index) const = 0;
};

}

