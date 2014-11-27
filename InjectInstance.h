#pragma once
#include <tuple>
#include <memory>
#include "InjectRegister.h"
#include "Inject.h"

namespace UnitTest
{

// InjectInstance is the "Instance" based implementation for Inject specializations.
// The Resolve function will create a new instance of the TInject class and use the
// Inject specializations to resolve each of the constructor arguments TArgs.
// NOTE: Derived class must define a default constructor for automatic registration
// with IFactory to properly work.
template <typename T, typename TInject, typename... TArgs>
class InjectInstance
{
	//nothing (should only ever use the tuple specialization)
};

template <typename T, typename TInject, typename... TArgs>
class InjectInstance<T, TInject, std::tuple<TArgs...>>
{
public:
	static InjectRegister<InjectInstance<T, TInject, std::tuple<TArgs...>>> mAutoRegister;

	virtual ~InjectInstance()
	{
//The following "seemingly" unused variable reference is required for automatic registration.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
		mAutoRegister;
	}
#pragma GCC diagnostic pop

	static std::shared_ptr<T> Resolve()
	{
		return std::shared_ptr<T>(new TInject(Inject<TArgs>::Resolve()...));
	}
};

//Template static member initialization
template <typename T, typename TInject, typename... TArgs>
InjectRegister<InjectInstance<T, TInject, std::tuple<TArgs...>>>
InjectInstance<T, TInject, std::tuple<TArgs...>>::mAutoRegister;

}

