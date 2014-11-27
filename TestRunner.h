#pragma once
#include "ITestRun.h"
#include "TestRepository.h"
#include <iostream>
#include <cstring>

namespace UnitTest
{

class TestRunner
{
public:
	static void RunTests(ITestRun& testRun)
	{
		auto& repository = TestRepository::GetInstance();
		unsigned long classCount = repository.GetCount();
		unsigned long methodCount = 0;
		for (unsigned long index = 0; index < classCount; ++index)
			methodCount += repository.Get(index).GetCount();
		testRun.OnInitialize(classCount, methodCount);

		unsigned long passedCount = 0;
		unsigned long failedCount = 0;
		for (unsigned long index = 0; index < classCount; ++index)
		{
			auto& factory = repository.Get(index);
			unsigned long count = factory.GetCount();
			testRun.OnBeginClass(factory.GetTestClassName(), count);

			if (!InitializeTests(testRun, factory))
				failedCount += count + 1;
			else
				for (unsigned long index = 0; index < count; ++index)
					if (RunTestMethod(testRun, factory, index))
						++passedCount;
					else
						++failedCount;

			if (!TerminateTests(testRun, factory))
				++failedCount;

			testRun.OnEndClass();
		}

		testRun.OnTerminate(passedCount, failedCount);
	}

	static void RunTestsFromCommandLine(int argc, char** argv)
	{
		if (argc == 2 && std::strcmp(argv[1], "PrintTests") == 0)
			PrintTests(std::cout);
		else if (argc == 4 && std::strcmp(argv[1], "RunSingleTest") == 0)
			RunSingleTest(argv[2], argv[3], std::cout);
	}

	static void PrintTests(std::ostream& out)
	{
		auto& repository = TestRepository::GetInstance();
		for (auto classIndex = 0ul; classIndex < repository.GetCount(); ++classIndex)
		{
			auto& classFactory = repository.Get(classIndex);
			for (auto methodIndex = 0ul; methodIndex < classFactory.GetCount(); ++methodIndex)
			{
				auto& methodFactory = classFactory.Get(methodIndex);
				out << methodFactory.GetTestMethodLocation() << ": "
					<< classFactory.GetTestClassName() << " "
					<< methodFactory.GetTestMethodName() << std::endl;
			}
		}
	}

	static void RunSingleTest(const std::string& className, const std::string& testMethodName, std::ostream& out)
	{
		try
		{
			auto& repository = TestRepository::GetInstance();
			auto foundClass = false;
			for (auto classIndex = 0ul; !foundClass && classIndex < repository.GetCount(); ++classIndex)
			{
				auto& classFactory = repository.Get(classIndex);
				if (className == classFactory.GetTestClassName())
				{
					foundClass = true;
					auto foundMethod = false;
					for (auto methodIndex = 0ul; !foundMethod && methodIndex < classFactory.GetCount(); ++methodIndex)
					{
						auto& methodFactory = classFactory.Get(methodIndex);
						if (testMethodName == methodFactory.GetTestMethodName())
						{
							foundMethod = true;
							classFactory.InitializeTests();
							auto instance = classFactory.CreateInstance();
							instance->BeginTest();
							methodFactory.CreateInstance(instance.get())->Execute();
							instance->EndTest();
							classFactory.TerminateTests();
							out << "Success" << std::endl;
						}
					}

					if (!foundMethod)
						out << "Failed: " << testMethodName << " unit test not found in " << className << " test class." << std::endl;
				}
			}

			if (!foundClass)
				out << "Failed: " << className << " test class not found." << std::endl;
		}
		catch (const std::exception& error)
		{
			out << "Failed: " << error.what() << std::endl;
		}
		catch (...)
		{
			out << "Failed: Unhandled exception." << std::endl;
		}
	}

private:
	static bool InitializeTests(ITestRun& testRun, ITestClassFactory& factory)
	{
		bool success = false;
		std::string reason;
		testRun.OnBeginMethod("[InitializeTests]");
		try
		{
			factory.InitializeTests();
			success = true;
		}
		catch (const std::exception& error)
		{
			reason = error.what();
		}
		catch (...)
		{
			reason = "Unhandled exception.";
		}
		testRun.OnEndMethod(success, reason);
		return success;
	}

	static bool TerminateTests(ITestRun& testRun, ITestClassFactory& factory)
	{
		bool success = false;
		std::string reason;
		testRun.OnBeginMethod("[TerminateTests]");
		try
		{
			factory.TerminateTests();
			success = true;
		}
		catch (const std::exception& error)
		{
			reason = error.what();
		}
		catch (...)
		{
			reason = "Unhandled exception.";
		}
		testRun.OnEndMethod(success, reason);
		return success;
	}

	static bool RunTestMethod(ITestRun& testRun, ITestClassFactory& factory, unsigned long index)
	{
		bool success = false;
		std::string reason;
		auto& mfactory = factory.Get(index);
		testRun.OnBeginMethod(mfactory.GetTestMethodName());
		try
		{
			auto instance = factory.CreateInstance();
			try
			{
				instance->BeginTest();
				mfactory.CreateInstance(instance.get())->Execute();
			}
			catch (...)
			{
				instance->EndTest();
				throw;
			}
			instance->EndTest();
			success = true;
		}
		catch (const std::exception& error)
		{
			reason = error.what();
		}
		catch (...)
		{
			reason = "Unhandled exception.";
		}
		testRun.OnEndMethod(success, reason);
		return success;
	}
};

}

