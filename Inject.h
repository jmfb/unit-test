#pragma once
#include <memory>

namespace UnitTest
{

// Template class used to resolve instances of a given interface.
// Note that this class is just a stub that will only produce compiler
// errors about missing Inject<T>::Resolve implementation.  All injection
// is done via template specialization of this class.
//
// Example:
//	//Specialization to allow for Inject<IFoo>::Resolve() to compile.
//	template <>
//	class Inject<IFoo>
//	{
//	public:
//		static std::shared_ptr<IFoo> Resolve()
//		{
//			return std::shared_ptr<IFoo>(new Foo());
//		}
//	};
template <typename T>
class Inject
{
public:
	static std::shared_ptr<T> Resolve();
};

}

