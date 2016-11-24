#include <iostream>
#include <string>
#include <memory>
#include <assert.h>

#include "Utils/CloneableContainers/Vector.h"
#include "Utils/CloneableContainers/PtrVector.h"
#include "Utils/CloneableContainers/RemovableVector.h"
#include "Utils/CloneableContainers/RemovablePtrVector.h"

using namespace Utils;

class Wrap1
{
public:
	explicit Wrap1(std::unique_ptr<int> v) : v_(std::move(v)) {}

	friend std::ostream& operator<<(std::ostream& os, const Wrap1 & v)
	{
		os << "[@" << v.v_.get() << "]: " << *v.v_;
		return os;
	}

	Wrap1 Clone() const
	{
		return Wrap1(std::make_unique<int>(*this->v_));
	}

private:
	std::unique_ptr<int> v_;
};

class Wrap2_Base
{
public:
	virtual ~Wrap2_Base() {}
	virtual std::unique_ptr<Wrap2_Base> Clone() const = 0;

	virtual void PrintDebug() const = 0;
	virtual int GetValue() const = 0;
};

class Wrap2 : public Wrap2_Base
{
public:
	explicit Wrap2(int v) : v_(v) {}
	Wrap2(const Wrap2& rhs) = default;

	std::unique_ptr<Wrap2_Base> Clone() const
	{
		return std::unique_ptr<Wrap2_Base>(new Wrap2(*this));
	}

	void PrintDebug() const
	{
		std::cout << "[@" << this << "]: " << v_ << std::endl;
	}

	int GetValue() const { return v_; }

private:
	int v_;
};

static void test9()
{
	CloneableContainers::RemovablePtrVector<Wrap2_Base*> container;

	auto r1 = container.PushBack(std::unique_ptr<Wrap2_Base>(new Wrap2(1)));
	auto r2 = container.PushBack(std::unique_ptr<Wrap2_Base>(new Wrap2(2)));
	auto r3 = container.PushBack(std::unique_ptr<Wrap2_Base>(new Wrap2(3)));

	int test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 1);
			return true;
		case 1:
			assert(item->GetValue() == 2);
			return true;
		case 2:
			assert(item->GetValue() == 3);
			return true;
		default:
			assert(false);
		}
	});

	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 1);
			return true;
		case 1:
			assert(item->GetValue() == 2);
			return false;
		default:
			assert(false);
		}
	});

	container.Remove(r2);
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 1);
			return true;
		case 1:
			assert(item->GetValue() == 3);
			return true;
		default:
			assert(false);
		}
	});
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 1);
			return true;
		case 1:
			assert(item->GetValue() == 3);
			return true;
		default:
			assert(false);
		}
	});

	auto r4 = container.PushBack(std::unique_ptr<Wrap2_Base>(new Wrap2(4)));
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 1);
			return true;
		case 1:
			assert(item->GetValue() == 3);
			return true;
		case 2:
			assert(item->GetValue() == 4);
			return true;
		default:
			assert(false);
		}
	});

	container.Remove(r1);
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 3);
			return true;
		case 1:
			assert(item->GetValue() == 4);
			return true;
		default:
			assert(false);
		}
	});
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		switch (test++) {
		case 0:
			assert(item->GetValue() == 3);
			return true;
		case 1:
			assert(item->GetValue() == 4);
			return true;
		default:
			assert(false);
		}
	});

	container.Remove(r1);
	container.Remove(r2);
	container.Remove(r3);
	container.Remove(r4);
	test = 0;
	container.IterateAll([&test](Wrap2_Base* item) -> bool {
		assert(false);
		return false;
	});
}

int main(void)
{
	test9();
	return 0;
}