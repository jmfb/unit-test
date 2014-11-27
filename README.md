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
			UnitTest::Mock<IFoo> mockFoo;
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

