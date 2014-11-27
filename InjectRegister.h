#pragma once
#include <functional>
#include "Inject.h"
#include "Factory.h"

namespace UnitTest
{

// InjectRegister is a helper class to automatically register Inject specialization Resolve
// functions with the singleton IFactory interface returned from Inject<IFactory>.
// NOTE: Requirements for InjectRegister to function properly:
//	1) Must be declared in a template class.
//	2) Must be declared as a static member.
//	3) Must be referenced with a virtual function of the template class.
//	4) Template class must be a base class of another class that also contains a default constructor.
// This causes the compiler to generate global static members whose constructors register types
// making the program appear as if this occurred during compilation (since it will occur during static
// variable initialization before any custom code executes).
template <typename T>
class InjectRegister
{
public:
	InjectRegister()
	{
		Inject<IFactory>::Resolve()->Register(std::function<decltype(T::Resolve)>(&T::Resolve));
	}
};

}

