#pragma once

#include "FlowControl/Result.h"
#include "FlowControl/ActionTypes.h"
#include "State/State.h"
#include "FlowControl/Helpers/EndTurn.h"
#include "FlowControl/Helpers/Resolver.h"
#include "FlowControl/FlowContext.h"
#include "FlowControl/IRandomGenerator.h"
#include "FlowControl/IActionParameterGetter.h"
#include "FlowControl/Context/AfterSummoned.h"
#include "FlowControl/Context/BattleCry.h"

// Implemention details which depends on manipulators
#include "FlowControl/Manipulators/HeroManipulator-impl.h"
#include "FlowControl/Manipulators/Helpers/AuraHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/EnchantmentHelper-impl.h"
#include "FlowControl/Manipulators/Helpers/OrderedCardsManager-impl.h"
#include "FlowControl/Manipulators/Helpers/DamageHelper-impl.h"
#include "State/Cards/Manager-impl.h"

namespace FlowControl
{
	class FlowController
	{
	public:
		FlowController(state::State & state, IActionParameterGetter & action_parameters, IRandomGenerator & random)
			: state_(state), action_parameters_(action_parameters), random_(random),
			flow_context_(random_, action_parameters_)
		{
		}

		Result PlayCard(int hand_idx)
		{
			PlayCardPhase(hand_idx);

			Result rc = Helpers::Resolver(state_, flow_context_).Resolve();
			assert(flow_context_.Empty());

			return rc;
		}

		Result EndTurn()
		{
			Helpers::EndTurnHelper helper(state_, flow_context_);
			Result rc = helper.Go();
			assert(flow_context_.Empty());
			return rc;
		}

		Result Attack(state::CardRef attacker, state::CardRef defender)
		{
			AttackPhase(attacker, defender);

			Result rc = Helpers::Resolver(state_, flow_context_).Resolve();
			assert(flow_context_.Empty());
			return rc;
		}

	private:
		void PlayCardPhase(int hand_idx)
		{
			state::CardRef card_ref = state_.GetCurrentPlayer().hand_.Get(hand_idx);
			state::Cards::Card const& card = state_.mgr.Get(card_ref);

			switch (card.GetCardType())
			{
			case state::kCardTypeMinion:
				return PlayMinionCardPhase(hand_idx, card_ref, card);
				break;
			default:
				throw std::exception("not implemented");
			}
		}

		void PlayMinionCardPhase(int hand_idx, state::CardRef card_ref, state::Cards::Card const& card)
		{
			state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeMinionSummoned>(
				Context::BeforeMinionSummoned(state_, card_ref, card));

			state_.GetCurrentPlayer().resource_.Cost(card.GetCost());

			if (state_.GetCurrentPlayer().minions_.Full()) throw std::exception("Minion full. Cannot play card.");

			int total_minions = (int)state_.GetCurrentPlayer().minions_.Size();
			int put_position = flow_context_.action_parameters_.GetMinionPutLocation(0, total_minions);

			Manipulate(state_, flow_context_).Minion(card_ref).Zone().ChangeTo<state::kCardZonePlay>(state_.current_player, put_position);

			FlowControl::Dispatchers::Minions::AfterSummoned(card.GetCardId(),
				Context::AfterSummoned(state_, flow_context_, card_ref, card));

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnMinionPlay>(card);

			FlowControl::Dispatchers::Minions::BattleCry(card.GetCardId(),
				Context::BattleCry(state_, card_ref, card, [this, card_ref, card]() {
				return flow_context_.action_parameters_.GetBattlecryTarget(state_, card_ref, card);
			}));

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterMinionPlayed>(card);

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterMinionSummoned>();
		}

	private:
		void AttackPhase(state::CardRef attacker, state::CardRef defender)
		{
			if (!attacker.IsValid()) return;
			if (!defender.IsValid()) return;

			while (true) {
				state::CardRef origin_attacker = attacker;
				state::CardRef origin_defender = defender;
				state_.event_mgr.TriggerEvent<state::Events::EventTypes::BeforeAttack>(state_, attacker, defender);

				if (!attacker.IsValid()) return;
				if (state_.mgr.Get(attacker).GetHP() <= 0) return;
				if (state_.mgr.Get(attacker).GetZone() != state::kCardZonePlay) return;

				if (!defender.IsValid()) return;
				if (state_.mgr.Get(defender).GetHP() <= 0) return;
				if (state_.mgr.Get(defender).GetZone() != state::kCardZonePlay) return;

				if (origin_attacker == attacker && origin_defender == defender) break;
			}

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnAttack>(
				Context::OnAttack(state_, flow_context_, attacker, defender));
			// TODO: attacker lose stealth

			Manipulate(state_, flow_context_).Character(defender).Damage().Take(state_.mgr.Get(attacker).GetAttack());
			Manipulate(state_, flow_context_).Character(attacker).Damage().Take(state_.mgr.Get(defender).GetAttack());

			state_.event_mgr.TriggerEvent<state::Events::EventTypes::AfterAttack>(state_, attacker, defender);

			{
				const state::Cards::Card & attacker_card = state_.mgr.Get(attacker);
				if (attacker_card.GetCardType() == state::kCardTypeHero) {
					state::CardRef weapon_ref = attacker_card.GetRawData().weapon_ref;
					if (weapon_ref.IsValid()) {
						Manipulate(state_, flow_context_).Card(weapon_ref).Damage().Take(1);
					}
				}
			}
		}

	public:
		state::State & state_;
		ActionParameterWrapper action_parameters_;
		IRandomGenerator & random_;
		FlowContext flow_context_;
	};
}