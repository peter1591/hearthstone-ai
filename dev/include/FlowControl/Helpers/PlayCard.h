#pragma once

#include "Manipulators/Manipulators.h"
#include "State/State.h"
#include "EventManager/StaticEvent.h"
#include "FlowControl/Result.h"
#include "FlowControl/Helpers/Utils.h"
#include "StaticDispatcher/MinionDispatcher.h"
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
				case Entity::kCardTypeMinion:
					return PlayMinionCard();
				}
				throw std::exception("not implemented");
			}

		private:
			Result PlayMinionCard()
			{
				Result rc = kResultNotDetermined;

				EventManager::StaticEvent<EventManager::TriggerTypes::BeforeMinionSummoned>::TriggerEvent(state_.event_mgr,
					Context::BeforeMinionSummoned(state_, card_ref_, *card_));

				state_.GetCurrentPlayer().resource_.Cost(card_->GetCost());

				int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();

				int put_position = 0;
				if (total_minions > 0) {
					put_position = action_parameters_.GetMinionPutLocation(0, total_minions);
				}

				state_.mgr.GetMinionManipulator(card_ref_).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(
					state_, state_.current_player, put_position);

				EventManager::StaticEvent<EventManager::TriggerTypes::OnMinionPlay>::TriggerEvent(state_.event_mgr, *card_);

				Cards::Minions::Dispatcher::BattleCry(card_->GetCardId(),
					Context::BattleCry(state_, card_ref_, *card_, [this]() {
					return this->GetBattelcryTarget();
				}));

				EventManager::StaticEvent<EventManager::TriggerTypes::AfterMinionPlayed>::TriggerEvent(state_.event_mgr, *card_);

				EventManager::StaticEvent<EventManager::TriggerTypes::AfterMinionSummoned>::TriggerEvent(state_.event_mgr);

				rc = Utils::CheckWinLoss(state_);
				if (rc != kResultNotDetermined) return rc;

				return kResultNotDetermined;
			}

			CardRef GetBattelcryTarget()
			{
				if (battlecry_target_.IsValid()) return battlecry_target_;
				throw std::exception("not implemented");
			}

		private:
			State::State & state_;
			ActionParameterGetter & action_parameters_;
			RandomGenerator & random_;

			CardRef card_ref_;
			const Entity::Card * card_;
			CardRef battlecry_target_;
		};
	}
}