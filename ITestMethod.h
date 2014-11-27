#pragma once

namespace UnitTest
{

class ITestMethod
{
public:
	virtual const char* GetTestMethodName() const = 0;
	virtual const char* GetTestMethodLocation() const = 0;
	virtual void Execute() const = 0;
};

}

