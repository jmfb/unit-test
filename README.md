# Introduction

The unit test framwork built for this project consists of three parts:

* Inversoin of Control (IoC) injection framework.
* Unit test harness framework.
* Mock object framework.

All of these features can be used via `#include <unit-test/UnitTest.h>` and using
the following examples in the `UnitTest` namespace.

# Inversion of Control

Start by declaring your interface.

```C++
namespace Example
{
	class Foo
	{
	public:
		virtual ~Foo() = default;
		virtual bool Func(int value) const = 0;
	};
	using FooPtr = std::shared_ptr<Foo>;
}
```

It is recommended that you put all of your code into your own namespace to
reduce name collisions but it is not required. Your interface should have
a virtual destructor since smart pointers will delete the class via the
interface. In addition you can put your abstract functions here. For ease
of use later it is also recommended you create a smart pointer typedef for
your interface.

Next, declare your class that implements the interface.

```C++
namespace Example
{
	class FooCore : public Foo
	{
	public:
		bool Func(int value) const override
		{
			return value == 69;
		}
	};
}
```

You will note that there is nothing special needed when declaring this class.

Next is the injection logic.

```C++
namespace UnitTest
{
	INJECT(Example::Foo, Example::FooCore, Instance, ());
}
```

The injection logic goes in the `UnitTest` namespace because the `INJECT` macro
expands into a template specialization of the `Inject` class for the given interface.
The parameters to `INJECT` are as follows:

Parameter | Value | Description
--------- | ----- | -----------
`Interface`	| `Example::Foo` | This is the interface whose implementation is being injected.
`Class` | `Example::FooCore` | This is the class that implements the interface.
`Lifetime` | `Instance` | This parameter can be `Instance` or `Singleton`. If instance, each request to resolve will result in a new instance. If singleton, each request to resolve will return the same instance.
`Constructor` | `()` | This is a signature that will be used to select which class constructor to use. Each argument, if present, must be an interface pointer that can also be injected.

Here is an example of the two possible usages.

```C++
bool InjectFooViaFactory(int value)
{
	auto factory = UnitTest::Inject<UnitTest::IFactory>::Resolve();
	auto foo = factory->Resolve<Example::Foo>();
	return foo->Func(value);
}
bool InjectFooViaInjectMap(int value)
{
	auto foo = UnitTest::Inject<Example::Foo>::Resolve();
	return foo->Func(value);
}
```

Each injected interface is registered with the `IFactory` singleton and can be resolved
via the interface. Alternatively, a client can use the `Inject` class directly to resolve
the interface but this does require the client to include the `INJECT` macros that have
been defined (whereas the `IFactory` method does not).

And finally, here is an example of a second class where `Foo` would be injected.

```C++
namespace Example
{
	class Bar
	{
	public:
		virtual ~Bar() = default;
		virtual int Func(int value) const = 0;
	};

	using BarPtr = std::shared_ptr<Bar>;

	class BarCore : public Bar
	{
	public:
		BarCore(FooPtr foo)
			: foo(foo)
		{
		}
		int Func(int value) const override
		{
			return foo->Func(value) ? value : -value;
		}
	private:
		FooPtr foo;
	};
}
namespace UnitTest
{
	INJECT(Example::Bar, Example::BarCore, Singleton, (Foo*));
}
```

# Test Harness

First, declare your test class and unit tests.

```C++
namespace Example
{
	TEST_CLASS(BarCoreTest)
	{
	public:
		BarCoreTest()
		{
		}
		static void InitializeTests()
		{
		}
		static void TerminateTests()
		{
		}
		void BeginTest() override
		{
		}
		void EndTest() override
		{
		}
		TEST_METHOD(FuncWhenFooTrue)
		{
			const auto value = 55;
			UnitTest::Mock<Foo> mockFoo;
			mockFoo.Setup(&Foo::Func, value).Returns(true);
			Bar bar{ mockFoo.GetObject() };
			auto result = bar.Func(value);
			Assert.AreEqual(value, result);
			mockFoo.Verify();
		}
	};
}
```

You will most likely want to put your test class in the same namespace as the class
you are testing. Recommended naming convention is the name of the class followed by `Test`.
You must define the default constructor (this is needed by part of the auto-registration
mechanism used for unit test discovery). You may optionally define any of the following
functions:

Note: If any of these functions fail, the unit tests will also be marked as failed even if they passed.

Function | When Called
-------- | -----------
`InitializeTests` | Called once before any unit tests in this class are called in order to initialize any static variables.
`TerminateTests` | Called once after all unit tests in this class have finished in order to clean up any static variables.
`BeginTest` | Called once before each unit test is run to initialize any non-static member variables.
`EndTest` | Called once after each unit test is run to clean up any non-static member variables.

Test classes are defined using the `TEST_CLASS` macro. This macro expands into a class for
getting the name of the test class and declares the named test class derived from the
`TestClass` template class which will perform auto-registration used for unit test discovery.

Test methods are defined using the `TEST_METHOD` macro. This macro expands into a class for
getting the name of the unit test and executing it given an instance of the test class.
It subsequently begins the method definition with the `void()` signature.

Next, you can run unit tests with the following code. Note that you could hook up the unit
test results into another unit test framework system by implementing a different `ITestRun`
implementation.

```C++
int main()
{
	UnitTest::TestRunWriter writer{ std::cout };
	UnitTest::TestRunner::RunTests(writer);
	return 0;
}
```

The following uses the built in command line interface for running specific tests.

```C++
int main(int argc, char* argv[])
{
	UnitTest::TestRunner::RunTestsFromCommandLine(argc, argv);
	return 0;
}
```

# Mock Objects

## Interface Mocking

You may have noticed the `Mock` usage in the previous example. The `Mock` template class allows
you to "mock" the implementation of a given interface by creating placeholder functions for
virtual function calls on the interface at run time.

Calls to `Setup` take a member function pointer to either the interface or a base interface.
This will be followed by N parameters where N is the number of arguments to the member function.
Each parameter must be convertible to the corresponding argument type or be the enum value
`UnitTest::Any::Match`. These parameters are used for argument list matching. The return value
from `Setup` is a `SetupData` class that can be used to further define the mock usage using the
following functions which can all be chained using fluent interfaces.

Function | Description
-------- | -----------
Returns | Sets the return value to the function. This function is only present if the return value of the function is not void.
Expects | Sets the number of calls this function should expect. The default value is 1.
Throws | Sets the exception that will be thrown when the function is invoked.
Callback | Sets a callback function (usually a lambda expression) to be called with the actual function arguments when the function is invoked. The parameters of the callback function must be convertible from the actual function parameters. Note that if `Throws` was specified the callback will occur prior to the exception being thrown.

```C++
TEST_METHOD(Func)
{
	UnitTest::Mock<Foo> mockFoo;
	mockFoo
		.Setup(&Foo::Func, 55)
		.Returns(true)
		.Expects(2)
		.Callback([](int value){ std::cout << "Callback " << value << std::endl; })
		.Throws(std::runtime_error{ "exception" });
	SomeFuncThatDoesAllOfThis(mockFoo.GetObject());
	mockFoo.Verify();
}
```

The `GetObject` function returns a smart pointer to the mocked interface where the deleter
object has been replaced with an empty lambda. This allows the fake interface to be used by
clients expecting the smart pointer but without causing the eventual call to delete on the
fake interface.

The `Verify` function verifies that all functions calls that were expected were called and
the correct number of times. This is not done in the `Mock` destructor automatically since
it would violate the design principle of "never throw from a destructor" (revisit when C++17
implements the `std::uncaught_exceptions` function).

If a function is called that is not mocked then an exception will be thrown stating there
was no mock implementation for the function at offset X where X is the index into the
v-table of the function that was called. This is as much information that is discernible
from an errant function call at run time.

## Internal Implementation of Mock Class

The mock class uses a few tricks to get run time interfaces to work.

First is the layout of classes that contain a v-table. This framework will work with any
compiler where the v-table is the first item of the class and the v-table is a series of
normal function pointers.

Second is the layout of class member functions. This framework will work with any compiler
where the member function is generated as a normal function with an additional first parameter
that is the `this` pointer to the class.

Thirdly is a compiler template expansion limitation for recursive template expansion.
The `"Const.h"` file contains a constant for the maximum number of virtual functions in a
class which is currently set at 50. This is needed because there is no current way to get
the offset of a function in the v-table for a class at compile time so a run time call must
be made with placeholders in place for all possible index values.

## API Mocking

```C++
BOOL __stdcall SomeApi(const APISTRUCT* in, APISTRUCT* out);
// ...
TEST_METHOD(CallsSomeApi)
{
	UnitTest::ApiMock<decltype(&::SomeApi), &::SomeApi> mockSomeApi(
		[](const APISTRUCT* in, APISTRUCT* out)
		{
			Assert.IsNotNull(in);
			Assert.AreEqual(100, in->value);
			Assert.IsNotNull(out);
			out->value = 200;
			return TRUE;
		});
	APISTRUCT in;
	APISTRUCT out;
	in.value = 100;
	auto result = ::SomeApi(&in, &out);
	Assert.AreEqual(TRUE, result);
	Assert.AreEqual(200, out.value);
}
```

The `ApiMock` class can be used to mock calls to API functions.  This is achieved by writing
a jump instruction to an intercept function in place of the original function.  Both x86 and
x64 instruction sets are supported.  Once the mock goes out of scope it will automatically
put back the original instructions that were overwritten and verify the mock was called.

This version of the mock does not support parameter matching, multiple setups, etc.  This is
because API interfaces don't generally support the type of interfaces where these make sense.
They have pointer arguments, output parameters, don't throw exceptions, etc.  For these reasons
the implementation only supports the callback version of a setup.  An `Expects` function is provided.

## Class Member Function Mocking

Mocking class member functions is supported though not by instance.

```C++
class Foo
{
public:
	int Func(int value) { /*...*/ }
};
// ...
TEST_METHOD(CallFunc)
{
	UnitTest::ClassMock<decltype(&Foo::Func), &Foo::Func> mockFooFunc(
		[](Foo* self, int value)
		{
			//self refers to the instance Foo whose Func call was intercepted.
			Assert.AreEqual(100, value);
			return 200;
		});
	Foo foo;
	auto result = foo.Func(100);
	Assert.AreEqual(200, result);
}
```

The `ClassMock` class can be used to mock calls to non-virtual class member functions.
This is acheived by using a detour to reroute calls to the member function to an
intercept function.  That function will in turn call the callback function with which
you constructed the mock.  The fluent interface supports `Expects`.

This version of the mock does not support parameter matching, multiple setups, etc.
This is primarily because these are not instance based mocks.  It would be possible
to extend this solution in that respect (even matching instance) but it would have
to extend the detour library to support call-through to the original function.
