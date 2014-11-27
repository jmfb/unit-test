#pragma once
#include <exception>
#include <string>
#include <sstream>

namespace UnitTest
{

class TestException : public std::exception
{
public:
	TestException(const std::string& message = "")
		: mMessage(message)
	{
	}
	~TestException() throw()
	{
	}

	virtual const char* what() const throw()
	{
		return mMessage.c_str();
	}

	static void Raise(const std::string& assertion, const std::string& message)
	{
		std::ostringstream out;
		out << assertion << ": " << message;
		throw TestException(out.str());
	}

	template <typename T1, typename T2>
	static void Raise(
		const std::string& assertion,
		const std::string& name1,
		const T1& value1,
		const std::string& name2,
		const T2& value2,
		const std::string& message)
	{
		std::ostringstream out;
		out << assertion << ": " << message
			<< " <" << name1 << ": " << value1
			<< "> <" << name2 << ": " << value2 << ">";
		throw TestException(out.str());
	}

private:
	std::string mMessage;
};

}

