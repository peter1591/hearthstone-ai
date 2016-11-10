#pragma once

#include <string>
#include "Entity/RawCard.h"

namespace Entity
{
	class Card
	{
	public:
		explicit Card(const RawCard & data) : data_(data) {}

	public: // getters and setters
		CardType GetCardType() const { return data_.card_type; }

		const std::string & GetCardId() const { return data_.card_id; }

		const CardZone GetZone() const { return data_.enchanted_states.zone; }
		void SetZone(CardZone new_zone) { data_.enchanted_states.zone = new_zone; }

		int GetZonePosition() const { return data_.zone_position; }
		void SetZonePosition(int pos) { data_.zone_position = pos; }

		int GetCost() const { return data_.enchanted_states.cost; }
		void SetCost(int new_cost) { data_.enchanted_states.cost = new_cost; }

		EnchantmentAuxData & GetMutableEnchantmentAuxData() { return data_.enchantment_aux_data; }
		AuraAuxData & GetMutableAuraAuxData() { return data_.aura_aux_data; }

	private:
		RawCard data_;
	};
}