#pragma once
#include <typeinfo>
#include <exception>
#include "Const.h"
#include "ForcedCast.h"
#include "VirtualTable.h"
#include "TestException.h"
#include "FunctionHelper.h"

namespace UnitTest
{

class OffsetHelper
{
public:
	OffsetHelper()
		: mValid(false), mOffset(0)
	{
		mTable.SetObject(this);
	}

	OffsetHelper(const OffsetHelper& rhs) = delete;
	OffsetHelper(OffsetHelper&& rhs) = delete;
	OffsetHelper& operator=(const OffsetHelper& rhs) = delete;
	OffsetHelper& operator=(OffsetHelper&& rhs) = delete;

	template <typename T>
	unsigned long GetOffset(T function);

	template <typename T>
	unsigned long GetOffsetDestructor();

	void SetOffset(unsigned long offset)
	{
		mValid = true;
		mOffset = offset;
	}

private:
	VirtualTable mTable;
	bool mValid;
	unsigned long mOffset;
};

template <unsigned long I>
inline void OffsetHelperFunctionPlaceholder(void* pThis)
{
	VirtualTable::GetObjectFromThis<OffsetHelper>(pThis)->SetOffset(I);
}

typedef void (*OffsetHelperFunction)(void*);

template <unsigned long I>
inline OffsetHelperFunction FindOffsetHelperFunction(unsigned long offset)
{
	if (offset == I)
		return &OffsetHelperFunctionPlaceholder<I>;
	return FindOffsetHelperFunction<I + 1>(offset);
}
template <>
inline OffsetHelperFunction FindOffsetHelperFunction<MAX_VIRTUAL_FUNCTIONS>(unsigned long offset)
{
	throw TestException("FindOffsetHelperFunction: Exceeded compiler template expansion limit.");
}

template <typename T>
inline unsigned long OffsetHelper::GetOffset(T function)
{
	typedef decltype(GetFunctionHelper(function)) FunctionHelper;
	typedef typename FunctionHelper::Class Class;

	mValid = false;
	mOffset = 0;
	for (unsigned long i = 0; i < MAX_VIRTUAL_FUNCTIONS; ++i)
		mTable.InstallFunction(i, FindOffsetHelperFunction<0>(i));
	(mTable.GetInterfacePtr<Class>()->*ForcedCast<void (Class::*)()>(function))();
	if (!mValid)
		throw TestException("OffsetHelper::GetOffset: Function was not virtual or exceeded limit.");
	return mOffset;
}

template <unsigned long I>
inline void OffsetHelperDestructorPlaceholder(void* pThis, void*)
{
	VirtualTable::GetObjectFromThis<OffsetHelper>(pThis)->SetOffset(I);
}

typedef void (*OffsetHelperDestructor)(void*, void*);

template <unsigned long I>
inline OffsetHelperDestructor FindOffsetHelperDestructor(unsigned long offset)
{
	if (offset == I)
		return &OffsetHelperDestructorPlaceholder<I>;
	return FindOffsetHelperDestructor<I + 1>(offset);
}
template <>
inline OffsetHelperDestructor FindOffsetHelperDestructor<MAX_VIRTUAL_FUNCTIONS>(unsigned long offset)
{
	throw TestException("FindOffsetHelperDestructor: Exceeded compiler template expansion limit.");
}

template <typename T>
inline unsigned long OffsetHelper::GetOffsetDestructor()
{
	mValid = false;
	mOffset = 0;
	for (unsigned long i = 0; i < MAX_VIRTUAL_FUNCTIONS; ++i)
		mTable.InstallFunction(i, FindOffsetHelperDestructor<0>(i));
	mTable.GetInterfacePtr<T>()->~T();
	if (!mValid)
		throw TestException("OffsetHelper::GetOffsetDestructor: Function was not virtual or exceeded limit.");
	return mOffset;
}

}

