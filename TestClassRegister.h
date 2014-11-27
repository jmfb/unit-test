#pragma once
#include "TestRepository.h"

namespace UnitTest
{

template <typename T>
class TestClassRegister
{
public:
	TestClassRegister()
	{
		TestRepository::GetInstance().Register<T>();
	}
};

}

