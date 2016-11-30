#pragma once

#include "State/State.h"
#include "FlowControl/Result.h"
#include "FlowControl/ActionParameterWrapper.h"
#include "FlowControl/Helpers/EntityDeathHandler.h"
#include "FlowControl/Dispatchers/Minions.h"
#include "FlowControl/Context/AfterSummoned.h"
#include "FlowControl/Context/BattleCry.h"
#include "FlowControl/Context/BeforeMinionSummoned.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/IRandomGenerator.h"
#include "FlowControl/ActionParameterWrapper.h"
#include "FlowControl/Manipulate.h"

namespace FlowControl
{
	namespace Helpers
	{
		class PlayCard
		{
		public:
			PlayCard(state::State & state, FlowContext & flow_context, int hand_idx)
				: state_(state), flow_context_(flow_context), hand_idx_(hand_idx), card_(nullptr)
			{

			}

			Result Go()
			{
				card_ref_ = state_.GetCurrentPlayer().hand_.Get(hand_idx_);
				card_ = &state_.mgr.Get(card_ref_);

				switch (card_->GetCardType())
				{
				case state::kCardTypeMinion:
					return PlayMinionCard();
				}
				throw std::exception("not implemented");
			}

		private:
			Result PlayMinionCard()
			{
				Result rc = kResultNotDetermined;

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
					Context::BeforeMinionSummoned(state_, card_ref_, *card_));

				state_.GetCurrentPlayer().resource_.Cost(card_->GetCost());

				int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
				int put_position = flow_context_.action_parameters_.GetMinionPutLocation(0, total_minions);

				Manipulate(state_, flow_context_).Minion(card_ref_).Zone().ChangeTo<state::kCardZonePlay>(state_.current_player, put_position);

				FlowControl::Dispatchers::Minions::AfterSummoned(card_->GetCardId(),
					Context::AfterSummoned(state_, flow_context_, card_ref_, *card_));

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnMinionPlay>(*card_);

				FlowControl::Dispatchers::Minions::BattleCry(card_->GetCardId(),
					Context::BattleCry(state_, card_ref_, *card_, [this]() {
						return flow_context_.action_parameters_.GetBattlecryTarget(state_, card_ref_, *card_);
				}));

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterMinionPlayed>(*card_);

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>();

				if ((rc = EntityDeathHandler(state_, flow_context_).ProcessDeath()) != kResultNotDetermined) return rc;

				return kResultNotDetermined;
			}

		private:
			state::State & state_;
			FlowContext & flow_context_;
			int hand_idx_;

			state::CardRef card_ref_;
			const state::Cards::Card * card_;
			state::CardRef battlecry_target_;
		};
	}
}