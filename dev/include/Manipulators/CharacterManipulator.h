#pragma once

#include "State/Cards/Card.h"
#include "State/Cards/Manager.h"
#include "State/CardRef.h"
#include "State/State.h"

namespace Manipulators
{
	class CharacterManipulator
	{
	public:
		CharacterManipulator(State::State & state, CardRef card_ref, Entity::Card & card)
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
		Entity::Card & card_;
		CardRef card_ref_;
	};
}