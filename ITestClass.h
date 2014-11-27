#pragma once

namespace UnitTest
{

class ITestClass
{
public:
	virtual const char* GetTestClassName() const = 0;
	virtual void BeginTest() = 0;
	virtual void EndTest() = 0;
};

}

