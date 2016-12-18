#pragma once

#include <string>
#include <utility>
#include "State/Cards/CardData.h"

namespace FlowControl
{
	namespace Manipulators
	{
		namespace detail
		{
			class DamageSetter;
		}

		namespace Helpers
		{
			class EnchantmentHelper;
			class AuraHelper;
			template <state::CardZone T1, state::CardType T2> class ZoneChanger;
			class ZonePositionSetter;
		}
	}
}

namespace state
{
	namespace Cards
	{
		class Card
		{
		public:
			class MutableEnchantmentAuxDataGetter
			{
				friend class FlowControl::Manipulators::Helpers::EnchantmentHelper;
			public:
				MutableEnchantmentAuxDataGetter(CardData & data) : data_(data) {}
			private:
				EnchantmentAuxData & Get() { return data_.enchantment_aux_data; }
			private:
				CardData & data_;
			};

			class MutableAuraAuxDataGetter
			{
				friend class FlowControl::Manipulators::Helpers::AuraHelper;
			public:
				MutableAuraAuxDataGetter(CardData & data) : data_(data) {}
			private:
				AuraAuxData & Get() { return data_.aura_aux_data; }
			private:
				CardData & data_;
			};

			class LocationSetter
			{
				template <state::CardZone T1, state::CardType T2> friend class FlowControl::Manipulators::Helpers::ZoneChanger;
				friend class FlowControl::Manipulators::Helpers::ZonePositionSetter;

			public:
				LocationSetter(CardData & data) : data_(data) {}
			private:
				LocationSetter & Player(PlayerIdentifier player)
				{
					data_.enchantable_states.player = player;
					return *this;
				}

				LocationSetter & Zone(CardZone new_zone)
				{
					data_.zone = new_zone;
					return *this;
				}

				LocationSetter & Position(int pos)
				{
					data_.zone_position = pos;
					return *this;
				}

			private:
				CardData & data_;
			};

			class DamageSetter
			{
				friend class FlowControl::Manipulators::detail::DamageSetter;
			public:
				DamageSetter(CardData & data) : data_(data) {}
			private:
				void Set(int v) { data_.damaged = v; }
			private:
				CardData & data_;
			};

		public:
			explicit Card(const CardData & data) : data_(data) {}

		public: // getters and setters
			int GetCardId() const { return data_.card_id; }
			CardType GetCardType() const { return data_.card_type; }
			CardRace GetRace() const { return data_.card_race; }
			int GetPlayOrder() const { return data_.play_order; }

			CardData::BattlecryCallback* GetBattlecryCallback() const { return data_.battlecry; }
			CardData::AddedToPlayZoneCallback* GetAddedToPlayZoneCallback() const { return data_.added_to_play_zone; }

			const PlayerIdentifier GetPlayerIdentifier() const { return data_.enchantable_states.player; }
			const CardZone GetZone() const { return data_.zone; }
			int GetZonePosition() const { return data_.zone_position; }

			int GetCost() const { return data_.enchantable_states.cost; }
			void SetCost(int new_cost) { data_.enchantable_states.cost = new_cost; }

			int GetDamage() const { return data_.damaged; }
			DamageSetter GetDamageSetter() { return DamageSetter(data_); }

			int GetHP() const { return data_.enchantable_states.max_hp - data_.damaged; }

			int GetAttack() const { return data_.enchantable_states.attack; }
			void SetAttack(int new_attack) { data_.enchantable_states.attack = new_attack; }

			int GetMaxHP() const { return data_.enchantable_states.max_hp; }
			void SetMaxHP(int max_hp) { data_.enchantable_states.max_hp = max_hp; }

			void SetTaunt(bool v) { data_.enchantable_states.taunt = v; }
			void SetShield(bool v) { data_.enchantable_states.shielded = v; }
			void SetCharge(bool v) { data_.enchantable_states.charge = v; }

			void IncreaseNumAttacksThisTurn() { ++data_.num_attacks_this_turn; }
			void ClearNumAttacksThisTurn() { data_.num_attacks_this_turn = 0; }

			void SetJustPlayedFlag(bool v) { data_.just_played = v; }

			MutableEnchantmentAuxDataGetter GetMutableEnchantmentAuxDataGetter()
			{
				return MutableEnchantmentAuxDataGetter(data_);
			}
			EnchantmentAuxData GetEnchantmentAuxData() const { return data_.enchantment_aux_data; }

			MutableAuraAuxDataGetter GetMutableAuraAuxDataGetter()
			{
				return MutableAuraAuxDataGetter(data_);
			}

			LocationSetter SetLocation() { return LocationSetter(data_); }

			template <typename T>
			void AddDeathrattle(T&& deathrattle) {
				data_.deathrattles.push_back(std::forward<T>(deathrattle));
			}

			CardData::Deathrattles & MutableDeathrattles() { return data_.deathrattles; }

		public:
			const CardData & GetRawData() const { return data_; }

		private:
			CardData data_;
		};
	}
}