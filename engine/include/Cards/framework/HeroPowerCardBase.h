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
			auto const& data = Cards::Database::GetInstance().Get(
				(Cards::CardId)CardClassIdMap<T>::id);
			GeneralCardBase<T>::Init(data);

			assert(data.card_type == state::kCardTypeHeroPower);

			this->enchanted_states.cost = data.cost;
		}
	};
}