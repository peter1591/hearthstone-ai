#pragma once

#include "Cards/framework/GeneralCardBase.h"
#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"

namespace Cards
{
	template <typename T>
	class SecretCardBase : public GeneralCardBase<T>
	{
	public:
		SecretCardBase()
		{
			this->card_id = CardClassIdMap<T>::id;

			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			assert(data.card_type == state::kCardTypeSecret);

			this->card_type = data.card_type;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
		}
	};
}