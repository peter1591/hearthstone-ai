#pragma once

#include <set>
#include <vector>

#include "state/State.h"
#include "FlowControl/FlowController.h"
#include "FlowControl/IActionParameterGetter.h"
#include "FlowControl/ValidActionGetter.h"
#include "FlowControl/utils/MainOp.h"

namespace FlowControl
{
	namespace utils {
		class ActionApplier
		{
		public:
			ActionApplier(std::vector<int> const& attackers, std::vector<size_t> const& playable_cards)
				: attackers_(attackers), playable_cards_(playable_cards)
			{
			}

			Result Apply(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{
				auto main_op = action.ChooseMainOp();

				switch (main_op) {
				case FlowControl::utils::MainOpType::kMainOpPlayCard:
					return PlayCard(state, action, random);

				case FlowControl::utils::MainOpType::kMainOpAttack:
					return Attack(state, action, random);

				case FlowControl::utils::MainOpType::kMainOpHeroPower:
					return HeroPower(state, action, random);

				case FlowControl::utils::MainOpType::kMainOpEndTurn:
					return EndTurn(state, action, random);

				default:
					assert(false);
					return Result::kResultInvalid;
				}
			}

		private:
			Result PlayCard(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{
				FlowContext flow_context(random, action);
				FlowController flow_controller(state, flow_context);

				assert(!playable_cards_.empty());
				int idx = 0;
				if (playable_cards_.size() >= 2) {
					idx = action.ChooseHandCard();
					if (idx < 0) return Result::kResultInvalid;
				}
				size_t hand_idx = playable_cards_[idx];

				return flow_controller.PlayCard((int)hand_idx);
			}

			Result Attack(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{
				assert([&]() {
					std::set<int> current;
					for (int idx : attackers_) current.insert(idx);
					std::set<int> checking;
					ValidActionGetter(state).ForEachAttacker([&](int idx) {
						checking.insert(idx);
						return true;
					});
					return current == checking;
				}());

				FlowContext flow_context(random, action);
				FlowController flow_controller(state, flow_context);

				if (attackers_.empty()) return Result::kResultInvalid;

				assert(!attackers_.empty());
				int idx = 0;
				if (attackers_.size() >= 2) {
					idx = action.ChooseAttacker(attackers_);
				}
				if (idx < 0) return Result::kResultInvalid;

				int attacker_idx = attackers_[idx];
				state::CardRef attacker = ValidActionGetter(state).GetFromAttackerIndex(attacker_idx);
				assert(attacker.IsValid());
				
				return flow_controller.Attack(attacker);
			}

			Result HeroPower(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{
				FlowContext flow_context(random, action);
				FlowController flow_controller(state, flow_context);
				return flow_controller.HeroPower();
			}

			Result EndTurn(state::State & state, IActionParameterGetter & action, state::IRandomGenerator & random)
			{
				FlowContext flow_context(random, action);
				FlowController flow_controller(state, flow_context);
				return flow_controller.EndTurn();
			}

		private:
			std::vector<int> const& attackers_;
			std::vector<size_t> const& playable_cards_;
		};
	}
}