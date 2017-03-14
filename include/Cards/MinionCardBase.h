#pragma once

#include "Cards/Database.h"
#include "state/Cards/Card.h"
#include "Cards/AuraHelper.h"
#include "Cards/EventRegister.h"

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
			assert(data.card_type == state::kCardTypeMinion);

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

		void Charge()
		{
			this->enchantable_states.charge = true;
		}

		void SpellDamage(int v)
		{
			this->enchantable_states.spell_damage = v;
		}

		template <typename... Types>
		AuraHelper<Types...> Aura() { return AuraHelper<Types...>(*this); }

		template <typename EventType, typename EventHandler>
		auto RegisterEvent() {
			return EventRegister<EventType, EventHandler>((state::Cards::CardData&)*this);
		}

		template <typename LifeTime, typename SelfPolicy, typename EventType, typename EventHandler>
		auto RegisterEvent() {
			return EventRegisterHelper<LifeTime, SelfPolicy, EventType, EventHandler>((state::Cards::CardData&)*this);
		}
	};
}