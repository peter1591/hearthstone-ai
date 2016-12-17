#pragma once

#include "Cards/Database.h"
#include "State/Cards/Card.h"

namespace Cards
{
	template <typename T>
	class MinionCardBase : public state::Cards::CardData
	{
	public:
		MinionCardBase()
		{
			this->card_id = T::id;

			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			this->card_type = data.card_type;
			this->card_race = data.card_race;
			this->card_rarity = data.card_rarity;

			this->enchantable_states.cost = data.cost;
			this->enchantable_states.attack = data.attack;
			this->enchantable_states.max_hp = data.max_hp;
		}

		void Taunt()
		{
			this->enchantable_states.taunt = true;
		}

		void Shield()
		{
			this->enchantable_states.shielded = true;
		}
	};
}