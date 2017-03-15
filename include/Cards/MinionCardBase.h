#pragma once

#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"
#include "Cards/EventRegister.h"
#include "Cards/BattlecryHelper.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/CardAttributes.h"

namespace Cards
{
	template <typename T, typename SpecifiedCardAttributes = NoAttribute>
	class MinionCardBase : public state::Cards::CardData, public MinionCardUtils
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

			this->enchantable_states.cost = data.cost;
			this->enchantable_states.attack = data.attack;
			this->enchantable_states.max_hp = data.max_hp;

			SpecifiedCardAttributes::Apply(*this);

			BattlecryProcessor<T>(*this);
		}

		template <typename... Types>
		auto Aura() { return AuraHelper<T, Types...>(*this); }

		template <typename EventType, typename EventHandler = T>
		auto RegisterEvent() {
			return EventRegister<EventType, EventHandler>((state::Cards::CardData&)*this);
		}

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler>
		auto RegisterEvent() {
			return EventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler>((state::Cards::CardData&)*this);
		}

		template <typename LifeTime, typename SelfPolicy, typename EventType>
		auto RegisterEvent() {
			return EventRegisterHelper<LifeTime, SelfPolicy, EventType, T>((state::Cards::CardData&)*this);
		}
	};
}