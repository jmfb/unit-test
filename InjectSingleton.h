#pragma once
#include <tuple>
#include <memory>
#include "InjectRegister.h"
#include "Inject.h"

namespace UnitTest
{

// InjectSingleton is the "Singleton" based implementation for Inject specializations.
// The Resolve function will create a new instance of the TInject class only upon
// the first call to resolve and use the Inject specializations to resolve each of
// the constructor arguments TArgs.  Each subsequent call will return the same instance.
// NOTE: Derived class must define a default constructor for automatic registration
// with IFactory to properly work.
template <typename T, typename TInject, typename... TArgs>
class InjectSingleton
{
	//nothing (should only ever use the tuple specialization)
};

template <typename T, typename TInject, typename... TArgs>
class InjectSingleton<T, TInject, std::tuple<TArgs...>>
{
public:
	static InjectRegister<InjectSingleton<T, TInject, std::tuple<TArgs...>>> mAutoRegister;

	virtual ~InjectSingleton()
	{
//The following "seemingly" unused variable reference is required for automatic registration.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-value"
		mAutoRegister;
	}
#pragma GCC diagnostic pop

	static std::shared_ptr<T> Resolve()
	{
		static std::shared_ptr<T> instance(new TInject(Inject<TArgs>::Resolve()...));
		return instance;
	}
};

//Template static member initialization
template <typename T, typename TInject, typename... TArgs>
InjectRegister<InjectSingleton<T, TInject, std::tuple<TArgs...>>>
InjectSingleton<T, TInject, std::tuple<TArgs...>>::mAutoRegister;

}

