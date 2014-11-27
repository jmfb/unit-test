#pragma once
#include <memory>
#include <typeinfo>
#include <exception>
#include <sstream>
#include <string>
#include "TestException.h"
#include "TypeName.h"

namespace UnitTest
{

class Any
{
public:
	Any()
	{
	}

	Any(const Any& rhs)
		: mContent(rhs.mContent ? rhs.mContent->Clone() : std::shared_ptr<Placeholder>())
	{
	}

	Any(Any&& rhs)
		: mContent(std::move(rhs.mContent))
	{
	}

	template <typename T>
	Any(T value)
		: mContent(new Holder<T>(value))
	{
	}

	template <typename T>
	static Any MakeMatchAny()
	{
		Any any;
		any.mContent = std::shared_ptr<Placeholder>(new MatchAny<T>());
		return any;
	}

	Any& operator=(const Any& rhs)
	{
		if (this != &rhs && mContent != rhs.mContent)
		{
			mContent.reset();
			if (rhs.mContent)
				mContent = rhs.mContent->Clone();
		}
		return *this;
	}

	Any& operator=(Any&& rhs)
	{
		if (this != &rhs)
			mContent = std::move(rhs.mContent);
		return *this;
	}

	template <typename T>
	Any& operator=(T value)
	{
		mContent = std::shared_ptr<Placeholder>(new Holder<T>(value));
		return *this;
	}

	template <typename T>
	void Set(T value)
	{
		mContent = std::shared_ptr<Placeholder>(new Holder<T>(value));
	}

	enum MatchEnum
	{
		Match = 0
	};

	template <typename T>
	T GetValue() const
	{
		if (!mContent)
			throw TestException("Any::GetValue: null pointer reference.");
		if (mContent->GetType() != typeid(T))
			throw TestException("Any::GetValue: type mismatch.");
		return dynamic_cast<const Holder<T>*>(mContent.get())->mValue;
	}

	bool operator==(const Any& rhs) const
	{
		if (!mContent)
			throw TestException("Any::operator==: null pointer reference (this).");
		if (!rhs.mContent)
			throw TestException("Any::operator==: null pointer reference (rhs).");
		if (mContent->GetType() != rhs.mContent->GetType())
			throw TestException("Any::operator==: type mismatch.");
		return mContent->IsMatchAny() || rhs.mContent->IsMatchAny() || mContent->IsEqual(rhs.mContent.get());
	}

	std::string ToString() const
	{
		if (!mContent)
			return "null";
		return mContent->ToString();
	}

	void Throw() const
	{
		if (mContent)
			mContent->Throw();
	}

	bool IsNull() const
	{
		return !mContent;
	}

private:
	class Placeholder
	{
	public:
		virtual ~Placeholder()
		{
		}

		virtual std::shared_ptr<Placeholder> Clone() const = 0;
		virtual const std::type_info& GetType() const = 0;
		virtual void Throw() const = 0;
		virtual bool IsEqual(const Placeholder* rhs) const = 0;
		virtual bool IsMatchAny() const = 0;
		virtual std::string ToString() const = 0;
	};

	template <typename T>
	class Holder : public Placeholder
	{
	public:
		Holder(T value)
			: mValue(value)
		{
		}
		virtual std::shared_ptr<Placeholder> Clone() const
		{
			return std::shared_ptr<Placeholder>(new Holder<T>(mValue));
		}
		virtual const std::type_info& GetType() const
		{
			return typeid(T);
		}
		virtual void Throw() const
		{
			throw mValue;
		}
		virtual bool IsEqual(const Placeholder* rhs) const
		{
			return mValue == dynamic_cast<const Holder<T>*>(rhs)->mValue;
		}
		virtual bool IsMatchAny() const
		{
			return false;
		}
		virtual std::string ToString() const
		{
			std::ostringstream out;
			out << TypeName<T>::Get() << "='" << mValue << "'";
			return out.str();
		}

		T mValue;
	};

	template <typename T>
	class Holder<T&> : public Placeholder
	{
	public:
		Holder(T& value)
			: mValue(value)
		{
		}
		virtual std::shared_ptr<Placeholder> Clone() const
		{
			return std::shared_ptr<Placeholder>(new Holder<T&>(mValue));
		}
		virtual const std::type_info& GetType() const
		{
			return typeid(T&);
		}
		virtual void Throw() const
		{
			std::ostringstream out;
			out << "Cannot re-throw " << TypeName<T>::Get() << "& type.";
			throw TestException(out.str());
		}
		virtual bool IsEqual(const Placeholder* rhs) const
		{
			return &mValue == &dynamic_cast<const Holder<T&>*>(rhs)->mValue;
		}
		virtual bool IsMatchAny() const
		{
			return false;
		}
		virtual std::string ToString() const
		{
			std::ostringstream out;
			out << TypeName<T>::Get() << "&='" << mValue << "'";
			return out.str();
		}

		T& mValue;
	};

	template <typename T>
	class MatchAny : public Placeholder
	{
	public:
		virtual std::shared_ptr<Placeholder> Clone() const
		{
			return std::shared_ptr<Placeholder>(new MatchAny<T>());
		}
		virtual const std::type_info& GetType() const
		{
			return typeid(T);
		}
		virtual void Throw() const
		{
		}
		virtual bool IsEqual(const Placeholder* rhs) const
		{
			return false;
		}
		virtual bool IsMatchAny() const
		{
			return true;
		}
		virtual std::string ToString() const
		{
			return "any";
		}
	};

	std::shared_ptr<Placeholder> mContent;
};

template <typename TOutput, typename TInput>
inline Any any_cast(TInput value)
{
	return Any(static_cast<TOutput>(value));
}

template <typename TOutput>
inline Any any_cast(Any::MatchEnum value)
{
	return Any::MakeMatchAny<TOutput>();
}

}

