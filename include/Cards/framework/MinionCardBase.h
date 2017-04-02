#pragma once

#include "Cards/framework/GeneralCardBase.h"
#include "Cards/Database.h"
#include "state/Cards/Card.h"

namespace Cards
{
	template <typename T,
		typename SpecifiedCardAttributes1 = NoAttribute,
		typename SpecifiedCardAttributes2 = NoAttribute,
		typename SpecifiedCardAttributes3 = NoAttribute,
		typename SpecifiedCardAttributes4 = NoAttribute,
		typename SpecifiedCardAttributes5 = NoAttribute>
		class MinionCardBase : public GeneralCardBase<T>, protected MinionCardUtils
	{
	public:
		MinionCardBase()
		{
			this->card_id = CardClassIdMap<T>::id;

			auto const& data = Cards::Database::GetInstance().Get(this->card_id);
			assert(data.card_type == state::kCardTypeMinion);

			this->card_type = data.card_type;
			this->card_race = data.card_race;
			this->card_rarity = data.card_rarity;

			this->enchanted_states.cost = data.cost;
			this->enchanted_states.attack = data.attack;
			this->enchanted_states.max_hp = data.max_hp;

			SpecifiedCardAttributes1::Apply(*this);
			SpecifiedCardAttributes2::Apply(*this);
			SpecifiedCardAttributes3::Apply(*this);
			SpecifiedCardAttributes4::Apply(*this);
			SpecifiedCardAttributes5::Apply(*this);

			BattlecryProcessor<T>(*this);
		}

	protected:
		template <typename... Types>
		auto AdjacentBuffAura() { return AdjacentBuffHelper<T, Types...>(*this); }
	};
}