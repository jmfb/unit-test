#pragma once
#include <memory>
#include "ITestClass.h"
#include "ITestMethod.h"

namespace UnitTest
{

class ITestMethodFactory
{
public:
	virtual const char* GetTestMethodName() const = 0;
	virtual const char* GetTestMethodLocation() const = 0;
	virtual std::shared_ptr<ITestMethod> CreateInstance(ITestClass* object) const = 0;
};

}

