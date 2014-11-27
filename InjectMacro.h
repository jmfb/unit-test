#pragma once
#include <functional>
#include "Inject.h"
#include "InjectInstance.h"
#include "InjectSingleton.h"
#include "TupleFromFunctionArgumentList.h"

// Helper macro to specialize the Inject class for a given interface/class injection.
// These definitions can occur anywhere in a project and the given interface can be
// resolved via the IFactory interface.  If you wish to use the Inject<Interface>::Resolve
// version to resolve the interface, the calling code will need to include the definitions
// since it will use template specialization to resolve (otherwise you will get a missing definition
// error for the default Inject<T>::Resolve function).
//
// Interface - The interface whose implementation is being injected.
// Class - The class that implements the interface (must be derived from Interface).
// Lifetime - The lifetime management of instance returned from Inject<>::Resolve.
//		Instance - Instance based.  Each call will result in a new instance.
//		Singleton - Singleton based.  Each call will return the same instance created on first use.
// ArgumentList - Parenthesis enclosing the parameters to inject into the given Class constructor.
//		() - Empty parenthesis to use the default constructor.
//		(A*,B*) - Otherwise, comma separated list of Interface pointers.
//			 	  NOTE: The need for this is because Macros cannot have variable arguments.
//				  The typelist is being deduced from a fake function call to this signature.
//				  The arguments have to be pointers because interfaces will have pure virtual functions.
//
// Example:
//	namespace UnitTest
//	{
//		// Inject IFoo interface using Foo class instance based with default constructor.
//		INJECT(IFoo, Foo, Instance, ());
//		// Inject IBar interface using Bar class singleton based with two instances of IFoo injected for constructor arguments.
//		INJECT(IBar, Bar, Singleton, (IFoo*, IFoo*));
//	}
//
// NOTE: The INJECT definitions must occur with the UnitTest namespace since they produce speicializations of that class.
// NOTE: The trailing semicolon is required since it was intentionally left off of the generated class declaration.
// NOTE: The inclusion of the Inject constructor in the macro expansion is solely for the purpose of causing the
//		 automatic registration with the IFactory interface; but is required nonetheless.
#define INJECT(Interface, Class, Lifetime, ArgumentList) \
	template <> \
	class Inject<Interface> : public Inject##Lifetime< \
		Interface, \
		Class, \
		TupleFromFunctionArgumentList<std::function<void ArgumentList>>::Type \
	> \
	{ \
	public: \
		Inject() \
		{ \
		} \
	}

