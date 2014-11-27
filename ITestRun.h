#pragma once
#include <string>

namespace UnitTest
{

class ITestRun
{
public:
	virtual void OnInitialize(unsigned long classCount, unsigned long totalMethodCount) = 0;
	virtual void OnBeginClass(const char* className, unsigned long methodCount) = 0;
	virtual void OnBeginMethod(const char* methodName) = 0;
	virtual void OnEndMethod(bool passed, const std::string& description) = 0;
	virtual void OnEndClass() = 0;
	virtual void OnTerminate(unsigned long passedCount, unsigned long failedCount) = 0;
};

}

