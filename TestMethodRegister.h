#pragma once
#include "TestClassFactory.h"
#include "TestMethodFactory.h"

namespace UnitTest
{

template <typename T, typename TMethod>
class TestMethodRegister
{
public:
	TestMethodRegister()
	{
		TestClassFactory<T>::GetInstance().Register(&TestMethodFactory<T, TMethod>::GetInstance());
	}
};

}

