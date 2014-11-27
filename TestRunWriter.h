#pragma once
#include <iostream>
#include "ITestRun.h"

namespace UnitTest
{

class TestRunWriter : public ITestRun
{
public:
	TestRunWriter(std::ostream& out)
		: mOut(out)
	{
	}

	virtual void OnInitialize(unsigned long classCount, unsigned long totalMethodCount)
	{
		mOut << classCount << " test class(es) containing a total of "
			<< totalMethodCount << " unit test(s)." << std::endl
			<< std::endl;
	}
	virtual void OnBeginClass(const char* className, unsigned long methodCount)
	{
		mOut << "TestClass: " << className << ", " << methodCount << " unit test(s)." << std::endl;
	}
	virtual void OnBeginMethod(const char* methodName)
	{
		mOut << methodName << "...";
	}
	virtual void OnEndMethod(bool passed, const std::string& description)
	{
		mOut << (passed ? "Passed" : "Failed") << std::endl;
		if (!passed)
			mOut << description << std::endl;
	}
	virtual void OnEndClass()
	{
		mOut << std::endl;
	}
	virtual void OnTerminate(unsigned long passedCount, unsigned long failedCount)
	{
		mOut << passedCount << " passed, " << failedCount << " failed." << std::endl;
	}

private:
	std::ostream& mOut;
};

}

