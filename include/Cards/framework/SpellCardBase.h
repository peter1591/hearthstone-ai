#pragma once

#include "Cards/framework/GeneralCardBase.h"
#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"

namespace Cards
{
	template <typename T, typename... Ts>
	class SpellCardBase : public GeneralCardBase<T, Ts...>
	{
	public:
		SpellCardBase()
		{
			auto const& data = Cards::Database::GetInstance().Get(
				(Cards::CardId)CardClassIdMap<T>::id);
			GeneralCardBase::Init(data);
			assert(data.card_type == state::kCardTypeSpell);
		}
	};
}