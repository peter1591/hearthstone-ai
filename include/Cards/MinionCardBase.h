#pragma once

#include "State/Cards/Card.h"

namespace Cards
{
	class MinionCardBase : public state::Cards::CardData
	{
	public:
		MinionCardBase(int id, int cost, int attack, int hp)
		{
			this->card_id = id;
			this->card_type = state::CardType::kCardTypeMinion;
			this->enchantable_states.cost = cost;
			this->enchantable_states.attack = attack;
			this->enchantable_states.max_hp = hp;
		}
	};
}