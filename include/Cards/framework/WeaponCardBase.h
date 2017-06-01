#pragma once

#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"
#include "Cards/framework/GeneralCardBase.h"

namespace Cards
{
	template <typename T, typename... Ts>
	class WeaponCardBase : public GeneralCardBase<T, Ts...>
	{
	public:
		WeaponCardBase()
		{
			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			assert(data.card_type == state::kCardTypeWeapon);

			this->card_type = data.card_type;
			this->card_race = data.card_race;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
			this->enchanted_states.attack = data.attack;
			this->enchanted_states.max_hp = data.max_hp;
		}
	};
}