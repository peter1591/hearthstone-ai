#include <iostream>
#include <utility>

template <int v>
struct StaticInvokable {
	template <typename... Args>
	static void Invoke(Args&...) {
		std::cout << "invoked: " << v << std::endl;
	}
};

struct NullInvokable {
	template <typename... Args> static void Invoke(Args&&...) {}
};

template <typename T, typename U>
struct CombineStaticInvokable {
	template <typename... Args>
	static void Invoke(Args&... args) {
		T::Invoke(args...);
		U::Invoke(args...);
	}
};

template <typename T = NullInvokable>
class InvokableChain
{
public:
	template <typename U>
	struct Combine {
		using type = InvokableChain<CombineStaticInvokable<T, U>>;
	};

	template <typename U>
	using Combined = typename Combine<U>::type;

	template <typename... Args>
	static void Invoke(Args&&... args) { return T::Invoke(std::forward<Args>(args)...); }
};

int main(void)
{
	typedef StaticInvokable<1> T1;
	typedef StaticInvokable<2> T2;

	InvokableChain<>::Combined<T1>::Combined<T2>::Invoke();
	//InvokableChain<>::Combine<T1>::Combine<T2>::Invoke();

	return 0;
}