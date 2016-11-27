#pragma once

#include "FlowControl/Result.h"
#include "FlowControl/Helpers/Utils.h"

#include "FlowControl/Manipulators/StateManipulator.h"

namespace FlowControl
{
	namespace Helpers
	{
		template <class ActionParameterGetter, class RandomGenerator>
		class OnTurnEnd
		{
		public:
			OnTurnEnd(state::State & state, ActionParameterGetter & action_parameters, RandomGenerator & random)
				: state_(state), action_parameters_(action_parameters), random_(random)
			{

			}

			Result Go()
			{
				Result rc = kResultNotDetermined;

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTurnEnd>();
				if ((rc = Utils::CheckWinLoss(state_)) != kResultNotDetermined) return rc;

				if (state_.turn == 89) return kResultDraw;
				++state_.turn;

				state_.ChangePlayer();

				state_.GetCurrentPlayer().resource_.IncreaseTotal();
				state_.GetCurrentPlayer().resource_.Refill();
				// TODO: overload

				state_.event_mgr.TriggerEvent<state::Events::EventTypes::OnTurnStart>();
				if ((rc = Utils::CheckWinLoss(state_)) != kResultNotDetermined) return rc;

				DrawCard();
				if ((rc = Utils::CheckWinLoss(state_)) != kResultNotDetermined) return rc;

				return kResultNotDetermined;
			}

		private:
			void DrawCard()
			{
				if (state_.GetCurrentPlayer().deck_.Empty())
				{
					return Fatigue();
				}

				int deck_count = (int) state_.GetCurrentPlayer().deck_.Size();
				int deck_idx = 0;
				if (deck_count > 1) deck_idx = random_.Get(deck_count);

				CardRef card_ref = state_.GetCurrentPlayer().deck_.Get(deck_idx);

				if (state_.GetCurrentPlayer().hand_.Full()) {
					Manipulators::StateManipulator(state_).Card(card_ref).Zone().ChangeTo<state::kCardZoneGraveyard>(state_.current_player);
				}
				else {
					Manipulators::StateManipulator(state_).Card(card_ref).Zone().ChangeTo<state::kCardZoneHand>(state_.current_player);
				}

				// TODO: trigger on-draw event (parameter: card_ref)
			}

			void Fatigue()
			{
				int damage = ++state_.GetCurrentPlayer().fatigue_damage_;

				Manipulators::StateManipulator(state_).CurrentHero().TakeDamage(damage);
			}

		private:
			state::State & state_;
			ActionParameterGetter & action_parameters_;
			RandomGenerator & random_;
		};
	}
}