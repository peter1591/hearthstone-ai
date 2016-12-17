#pragma once

#include "Cards/Database.h"
#include "State/Cards/Card.h"

namespace Cards
{
	class MinionCardBase : public state::Cards::CardData
	{
	public:
		MinionCardBase(int id)
		{
			this->card_id = id;

			auto const& data = Cards::Database::GetInstance().Get(id);
			this->card_type = data.card_type;
			this->enchantable_states.cost = data.cost;
			this->enchantable_states.attack = data.attack;
			this->enchantable_states.max_hp = data.max_hp;
		}
	};
}