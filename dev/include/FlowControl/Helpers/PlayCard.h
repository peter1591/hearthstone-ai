#pragma once

#include "State/Manipulators/Manipulators.h"
#include "State/State.h"
#include "State/Events/StaticEvent.h"
#include "FlowControl/Result.h"
#include "FlowControl/Helpers/Utils.h"
#include "FlowControl/Helpers/ActionParameterWrapper.h"
#include "FlowControl/Dispatchers/Minions.h"
#include "FlowControl/Context/BattleCry.h"
#include "FlowControl/Context/BeforeMinionSummoned.h"

namespace FlowControl
{
	namespace Helpers
	{
		template <class ActionParameterGetter, class RandomGenerator>
		class PlayCard
		{
		public:
			PlayCard(State::State & state, ActionParameterGetter & action_parameters, RandomGenerator & random)
				: state_(state), action_parameters_(action_parameters), random_(random), card_(nullptr)
			{

			}

			Result Go()
			{
				int hand_pos = action_parameters_.GetHandCardPosition();
				card_ref_ = state_.GetCurrentPlayer().hand_.Get(hand_pos);
				card_ = &state_.mgr.Get(card_ref_);

				switch (card_->GetCardType())
				{
				case State::kCardTypeMinion:
					return PlayMinionCard();
				}
				throw std::exception("not implemented");
			}

		private:
			Result PlayMinionCard()
			{
				Result rc = kResultNotDetermined;

				State::Events::StaticEvent<State::Events::TriggerTypes::BeforeMinionSummoned>::TriggerEvent(state_.event_mgr,
					Context::BeforeMinionSummoned(state_, card_ref_, *card_));

				state_.GetCurrentPlayer().resource_.Cost(card_->GetCost());

				int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
				int put_position = action_parameters_.GetMinionPutLocation(0, total_minions);

				State::Manipulators::StateManipulator(state_).Minion(card_ref_)
					.Zone().ChangeTo<State::kCardZonePlay>(state_.current_player, put_position);

				State::Events::StaticEvent<State::Events::TriggerTypes::OnMinionPlay>::TriggerEvent(state_.event_mgr, *card_);

				FlowControl::Dispatchers::Minions::BattleCry(card_->GetCardId(),
					Context::BattleCry(state_, card_ref_, *card_, [this]() {
						return action_parameters_.GetBattlecryTarget(state_, card_ref_, *card_);
				}));

				State::Events::StaticEvent<State::Events::TriggerTypes::AfterMinionPlayed>::TriggerEvent(state_.event_mgr, *card_);

				State::Events::StaticEvent<State::Events::TriggerTypes::AfterMinionSummoned>::TriggerEvent(state_.event_mgr);

				rc = Utils::CheckWinLoss(state_);
				if (rc != kResultNotDetermined) return rc;

				return kResultNotDetermined;
			}

		private:
			State::State & state_;
			ActionParameterWrapper<ActionParameterGetter> action_parameters_;
			RandomGenerator & random_;

			CardRef card_ref_;
			const State::Cards::Card * card_;
			CardRef battlecry_target_;
		};
	}
}