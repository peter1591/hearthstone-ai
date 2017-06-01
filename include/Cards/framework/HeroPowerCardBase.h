#pragma once

#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"
#include "Cards/framework/GeneralCardBase.h"

namespace Cards
{
	template <typename T>
	class HeroPowerCardBase : public GeneralCardBase<T>
	{
	public:
		HeroPowerCardBase()
		{
			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			assert(data.card_type == state::kCardTypeHeroPower);

			this->card_type = data.card_type;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
		}
	};
}