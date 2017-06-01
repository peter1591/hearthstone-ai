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
			auto const& data = Cards::Database::GetInstance().Get(
				(Cards::CardId)CardClassIdMap<T>::id);
			GeneralCardBase::Init(data);
			assert(data.card_type == state::kCardTypeWeapon);
		}
	};
}