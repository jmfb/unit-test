#pragma once
#include "Const.h"
#include "ForcedCast.h"

namespace UnitTest
{

class VirtualTable
{
public:
	VirtualTable()
		: mObject(0)
	{
		for (unsigned long i = 0; i < MAX_VIRTUAL_FUNCTIONS; ++i)
			InstallFunction(i, &VirtualTable::NoOperation);
		mVirtualTablePtr = &mTable[0];
	}

	void SetObject(void* object)
	{
		mObject = object;
	}

	static void NoOperation(void*)
	{
	}

	template <typename T>
	void InstallFunction(unsigned long index, T function)
	{
		mTable[index] = ForcedCast<void*, T>(function);
	}

	template <typename T>
	T* GetInterfacePtr()
	{
		return reinterpret_cast<T*>(&mVirtualTablePtr);
	}

	template <typename T>
	static T* GetObjectFromThis(void* pThis)
	{
		auto vtable = *reinterpret_cast<void**>(pThis);
		auto vtclass = reinterpret_cast<VirtualTable*>(vtable);
		return reinterpret_cast<T*>(vtclass->mObject);
	}

private:
	void* mTable[MAX_VIRTUAL_FUNCTIONS];
	void* mVirtualTablePtr;
	void* mObject;
};

}

