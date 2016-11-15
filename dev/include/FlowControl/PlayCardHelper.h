#pragma once

#include "Manipulators/Manipulators.h"
#include "State/State.h"

namespace FlowControl
{
	template <class ActionParameterGetter, class RandomGenerator>
	class PlayCardHelper
	{
	public:
		PlayCardHelper(State::State & state, ActionParameterGetter & action_parameters, RandomGenerator & random)
			: state_(state), action_parameters_(action_parameters), random_(random), card_(nullptr)
		{

		}

		void PlayCard()
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
		void PlayMinionCard()
		{
			// TODO: before summon phase

			state_.GetCurrentPlayer().resource_.Cost(card_->GetCost());

			int total_minions = (int) state_.GetCurrentPlayer().minions_.Size();

			int put_position = 0;
			if (total_minions > 0) {
				put_position = action_parameters_.GetMinionPutLocation(0, total_minions);
			}

			state_.mgr.GetMinionManipulator(card_ref_).GetZoneChanger().ChangeTo<Entity::kCardZonePlay>(
				state_, state_.current_player, put_position);

			// TODO: on play phase
			// TODO: battlecry phase
			// TODO: after play phase
			// TODO: after summon phase
			// TODO: check win/loss
		}

	private:
		State::State & state_;
		ActionParameterGetter & action_parameters_;
		RandomGenerator & random_;

		CardRef card_ref_;
		const Entity::Card * card_;
	};
}