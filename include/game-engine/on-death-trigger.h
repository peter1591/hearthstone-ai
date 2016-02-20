#pragma once

#include <functional>

namespace GameEngine {
	template <typename... Params>
	class OnDeathTrigger {
		friend std::hash<OnDeathTrigger<Params...>>;

	public:
		typedef void Func(Params&&... params);

		OnDeathTrigger(Func * func) : func(func) {}

		bool operator==(OnDeathTrigger const& rhs) const {
			if (this->func != rhs.func) return false;
			return true;
		}
		bool operator!=(OnDeathTrigger const& rhs) const { return !(*this == rhs); }

		Func * func;
	};
}

namespace std {
	template <typename... Params>
	struct hash<GameEngine::OnDeathTrigger<Params...>> {
		typedef GameEngine::OnDeathTrigger<Params...> argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<decltype(s.func)>()(s.func);
		}
	};
}