#include <iostream>
#include <vector>
#include <chrono>
#include <tuple>

template <int v>
class Foo
{
public:
	Foo(volatile double& value) : value_(value) {}

	void Do() { value_ += v; }
	void Print() { std::cout << "[" << v << "]: " << value_ << std::endl; }

private:
	volatile double& value_;
};

class A
{
public:
	A(volatile double& v1, volatile double& v2, volatile double& v3, volatile double& v4, volatile double& v5) :
		item1(v1),
		item2(v2),
		item3(v3),
		item4(v4),
		item5(v5)
	{}

	void Do1() { item1.Do(); }
	void Do2() { item2.Do(); }
	void Do3() { item3.Do(); }
	void Do4() { item4.Do(); }
	void Do5() { item5.Do(); }

	void Print1() { item1.Print(); }
	void Print2() { item2.Print(); }
	void Print3() { item3.Print(); }
	void Print4() { item4.Print(); }
	void Print5() { item5.Print(); }

private:
	Foo<1> item1;
	Foo<2> item2;
	Foo<3> item3;
	Foo<4> item4;
	Foo<5> item5;
};

template <typename... Items>
class B
{
public:
	template <typename... Args>
	B(Args&&... args) : items(std::forward<Args>(args)...) {}

	template <int n>
	void Do()
	{
		std::get<n>(items).Do();
	}

	template <int n>
	void Print() {
		std::get<n>(items).Print();
	}

public:
	std::tuple<Items...> items;
};

int main(void)
{
	std::cout << "hello" << std::endl;

	constexpr static long times = 1000000000;

	while (true)
	{
		volatile double values[5]{};
		A a(values[0], values[1], values[2], values[3], values[4]);

		auto start = std::chrono::steady_clock::now();
		for (long i = 0; i < times; ++i) {
			a.Do1();
			a.Do2();
			a.Do3();
			a.Do4();
			a.Do5();
		}
		int ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

		a.Print1();
		a.Print2();
		a.Print3();
		a.Print4();
		a.Print5();
		std::cout << "original: elapsed: " << ms << " ms" << std::endl;

		values[0] = 0;
		values[1] = 0;
		values[2] = 0;
		values[3] = 0;
		values[4] = 0;
		B<Foo<1>, Foo<2>, Foo<3>, Foo<4>, Foo<5>> b(values[0], values[1], values[2], values[3], values[4]);
		start = std::chrono::steady_clock::now();
		for (long i = 0; i < times; ++i) {
			b.Do<0>();
			b.Do<1>();
			b.Do<2>();
			b.Do<3>();
			b.Do<4>();
		}
		ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count();

		b.Print<0>();
		b.Print<1>();
		b.Print<2>();
		b.Print<3>();
		b.Print<4>();
		std::cout << "tuple: elapsed: " << ms << " ms" << std::endl;
	}
	return 0;
}