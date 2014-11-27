#pragma once
#include <typeinfo>
#include <string>
#if defined(__GNUG__)
#include <cstdlib>
#include <cxxabi.h>
#endif

namespace UnitTest
{

template <typename T>
class TypeName
{
public:
	static std::string Get()
	{
#if defined(__GNUG__)
		int status = 0;
		char* realname = abi::__cxa_demangle(typeid(T).name(), 0, 0, &status);
		std::string retval = realname;
		::free(realname);
		return retval;
#else
		return typeid(T).name();
#endif
	}
};

}

