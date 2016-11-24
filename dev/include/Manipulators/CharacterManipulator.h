#pragma once

#include "State/Types.h"
#include "State/State.h"
#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"

namespace Manipulators
{
	class CharacterManipulator
	{
	public:
		CharacterManipulator(State::State & state, CardRef card_ref, State::Cards::Card & card)
			: state_(state), card_(card), card_ref_(card_ref)
		{
		}

		CharacterManipulator & TakeDamage(int damage)
		{
			card_.SetDamage(card_.GetDamage() + damage);
			return *this;
		}

	private:
		State::State & state_;
		State::Cards::Card & card_;
		CardRef card_ref_;
	};
}