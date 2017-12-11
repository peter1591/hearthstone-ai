#pragma once

namespace Utils {
	struct StaticNullInvokable {
		template <typename... Args> static void Invoke(Args&&...) {}
	};

	namespace detail {
		template <typename T, typename U>
		struct CombineStaticInvokable {
			template <typename... Args>
			static void Invoke(Args&&... args) {
				T::Invoke(args...);
				U::Invoke(args...);
			}
		};
	}

	template <typename T = StaticNullInvokable>
	class StaticInvokableChain
	{
	public:
		template <typename U>
		struct Combine {
			using type = StaticInvokableChain<detail::CombineStaticInvokable<T, U>>;
		};

		template <typename U>
		using CombineType = typename Combine<U>::type;

		template <typename... Args>
		static void Invoke(Args&&... args) { return T::Invoke(std::forward<Args>(args)...); }
	};
}