#pragma once
#include <typeinfo>
#include <functional>
#include <memory>
#include <sstream>
#include "TestException.h"
#include "TypeName.h"
#include "Mock.h"

namespace UnitTest
{

// IFactory interface injection interface.  This interface merges the type generic
// virtual functions of DoRegister and DoResolve with template functions that ensure
// compile time type safety.
//
// Example:
//	IFactory* factory = GetFactory();
//
//	//Resolve an interface via previous calls to Register or RegisterObject.
//	std::shared_ptr<IFoo> foo = factory->Resolve<IFoo>();
//
//	//Register a mock instance that will be returned on subsequent calls to Resolve.
//	MockBar mockBar;
//	factory->RegisterObject<IBar>(mockBar);
class IFactory
{
private:
	virtual void DoRegister(const std::type_info& type, std::function<void*()> resolver) = 0;
	virtual void DoUnregister(const std::type_info& type) = 0;
	virtual void* DoResolve(const std::type_info& type) const = 0;

public:
	template <typename T>
	void Register(std::function<std::shared_ptr<T>()> resolver)
	{
		DoRegister(typeid(T), [=]() -> void*
		{
			return new std::shared_ptr<T>(resolver());
		});
	}

	template <typename T>
	void Unregister()
	{
		DoUnregister(typeid(T));
	}

	template <typename T>
	void RegisterObject(T& object)
	{
		DoRegister(typeid(T), [&]() -> void*
		{
			return new std::shared_ptr<T>(&object, [](T*){});
		});
	}
	template <typename T>
	void RegisterObject(Mock<T>& mockObject)
	{
		DoRegister(typeid(T), [&]() -> void*
		{
			return new std::shared_ptr<T>(mockObject.GetObject().get(), [](T*){});
		});
	}

	template <typename T>
	std::shared_ptr<T> Resolve() const
	{
		void* result = DoResolve(typeid(T));
		if (result == nullptr)
		{
			std::ostringstream out;
			out << "IFactory::Resolve: Could not resolve type " << TypeName<T>::Get();
			throw TestException(out.str());
		}
		std::shared_ptr<std::shared_ptr<T>> pointer;
		pointer.reset(reinterpret_cast<std::shared_ptr<T>*>(result));
		return *pointer;
	}
};

typedef std::shared_ptr<IFactory> IFactoryPtr;

}

