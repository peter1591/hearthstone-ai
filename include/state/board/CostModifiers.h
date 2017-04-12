#pragma once

#include <variant>
#include <vector>

namespace state {
	namespace board {
		class BiasCost {
		public:
			BiasCost(int v) : v_(v), minions_only_(false) {}

			void Apply(state::CardType card_type, int * cost, bool * cost_health_instead) const {
				if (minions_only_ && card_type != CardType::kCardTypeMinion) return;
				*cost += v_;
			}

		private:
			int v_;
			bool minions_only_;
			state::PlayerIdentifier player_only_;
		};
		
		class SetCost {
		public:
			SetCost(int v) : v_(v) {}

			void Apply(state::CardType card_type, int * cost, bool * cost_health_instead) const {
				*cost = v_;
			}

		private:
			int v_;
		};

		using CostModifier = std::variant<
			BiasCost,
			SetCost>;

		class CostModifiers
		{
		public:
			template <typename T> void Add(T&& item) { items_.push_back(std::forward<T>(item)); }

			void Apply(state::CardType card_type, int * cost, bool * cost_health_instead) {
				for (auto const& item : items_) {
					std::visit([&](auto const& modifier) {
						modifier.Apply(card_type, cost, cost_health_instead);
					}, item);
				}
			}

		private:
			std::vector<CostModifier> items_;
		};
	}
}