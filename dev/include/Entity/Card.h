#pragma once

#include <string>
#include "Entity/RawCard.h"

namespace Manipulators
{
	namespace Helpers
	{
		class EnchantmentHelper;
		class AuraHelper;
		class MinionZoneChanger;
		class ZonePositionSetter;
	}
}

namespace Entity
{
	class Card
	{
	public:
		class MutableEnchantmentAuxDataGetter
		{
			friend class Manipulators::Helpers::EnchantmentHelper;
		public:
			MutableEnchantmentAuxDataGetter(RawCard & data) : data_(data) {}
		private:
			EnchantmentAuxData & Get() { return data_.enchantment_aux_data; }
		private:
			RawCard & data_;
		};

		class MutableAuraAuxDataGetter
		{
			friend class Manipulators::Helpers::AuraHelper;
		public:
			MutableAuraAuxDataGetter(RawCard & data) : data_(data) {}
		private:
			AuraAuxData & Get() { return data_.aura_aux_data; }
		private:
			RawCard & data_;
		};

		class LocationSetter
		{
			friend class Manipulators::Helpers::MinionZoneChanger;
			friend class Manipulators::Helpers::ZonePositionSetter;
		public:
			LocationSetter(RawCard & data) : data_(data) {}
		private:
			void SetPlayerIdentifier(State::PlayerIdentifier player) { data_.enchanted_states.player = player; }
			void SetZone(CardZone new_zone) { data_.enchanted_states.zone = new_zone; }
			void SetZonePosition(int pos) { data_.zone_position = pos; }

		private:
			RawCard & data_;
		};

	public:
		explicit Card(const RawCard & data) : data_(data) {}

	public: // getters and setters
		const std::string & GetCardId() const { return data_.card_id; }
		CardType GetCardType() const { return data_.card_type; }

		const State::PlayerIdentifier GetPlayerIdentifier() const { return data_.enchanted_states.player; }
		const CardZone GetZone() const { return data_.enchanted_states.zone; }
		int GetZonePosition() const { return data_.zone_position; }

		int GetCost() const { return data_.enchanted_states.cost; }
		void SetCost(int new_cost) { data_.enchanted_states.cost = new_cost; }

		MutableEnchantmentAuxDataGetter GetMutableEnchantmentAuxDataGetter()
		{
			return MutableEnchantmentAuxDataGetter(data_);
		}

		MutableAuraAuxDataGetter GetMutableAuraAuxDataGetter()
		{
			return MutableAuraAuxDataGetter(data_);
		}

		LocationSetter GetLocationSetter()
		{
			return LocationSetter(data_);
		}

	private:
		RawCard data_;
	};
}