#pragma once

#include <string>
#include <utility>
#include "state/Cards/CardData.h"

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
			class FlagAuraHelper;
			class DeathrattlesHelper;
			class TransformHelper;
		}
	}
}

namespace state
{
	template <CardZone, CardType> class ZoneChanger;
	namespace detail {
		class ZonePositionSetter;
		template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer;
	}

	namespace Cards
	{
		class Manager;
		class Card
		{
		public:
			class MutableAuraHandlerGetter
			{
				friend class FlowControl::Manipulators::Helpers::AuraHelper;
				friend class FlowControl::Manipulators::Helpers::TransformHelper;
			public:
				MutableAuraHandlerGetter(CardData & data) : data_(data) {}
			private:
				auto& Get() { return data_.aura_handler; }
			private:
				CardData & data_;
			};
			class MutableFlagAuraHandlerGetter
			{
				friend class FlowControl::Manipulators::Helpers::FlagAuraHelper;
				friend class FlowControl::Manipulators::Helpers::TransformHelper;
			public:
				MutableFlagAuraHandlerGetter(CardData & data) : data_(data) {}
			private:
				auto& Get() { return data_.flag_aura_handler; }
			private:
				CardData & data_;
			};

			class ZoneSetter {
				template <CardZone, CardType> friend class ZoneChanger;
			public:
				ZoneSetter(CardData & data) : data_(data) {}
			private:
				void operator()(PlayerIdentifier player, CardZone new_zone) {
					data_.enchanted_states.player = player;
					Zone(new_zone);
				}
				void Zone(CardZone new_zone) {
					data_.zone = new_zone;
				}
			private:
				CardData & data_;
			};

			class ZonePosSetter {
				friend state::Cards::Manager;
				template <CardZone, CardType> friend class state::ZoneChanger;
				template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;
			public:
				ZonePosSetter(CardData & data) : data_(data) {}
			private:
				void operator()(int pos) {
					data_.zone_position = pos;
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
			explicit Card(CardData&& data) : data_(std::move(data)) {}

		public: // getters and setters
			int GetCardId() const { return data_.card_id; }
			CardType GetCardType() const { return data_.card_type; }
			CardRace GetRace() const { return data_.card_race; }
			int GetPlayOrder() const { return data_.play_order; }

			const PlayerIdentifier GetPlayerIdentifier() const { return data_.enchanted_states.player; }
			const CardZone GetZone() const { return data_.zone; }
			int GetZonePosition() const { return data_.zone_position; }

			int GetCost() const { return data_.enchanted_states.cost; }
			void SetCost(int new_cost) { data_.enchanted_states.cost = new_cost; }

			int GetDamage() const { return data_.damaged; }
			DamageSetter GetDamageSetter() { return DamageSetter(data_); }

			int GetHP() const { return data_.enchanted_states.max_hp - data_.damaged; }

			int GetAttack() const { return data_.enchanted_states.attack; }
			void SetAttack(int new_attack) { data_.enchanted_states.attack = new_attack; }

			int GetMaxHP() const { return data_.enchanted_states.max_hp; }
			void SetMaxHP(int max_hp) { data_.enchanted_states.max_hp = max_hp; }

			void SetTaunt(bool v) { data_.taunt = v; }
			bool HasTaunt() const { return data_.taunt; }

			void SetShield(bool v) { data_.shielded = v; }
			bool hasShield() const { return data_.shielded; }

			void SetCharge(bool v) { data_.charge = v; }
			bool HasCharge() const { return data_.charge; }

			int GetSpellDamage() const { return data_.enchanted_states.spell_damage; }
			void SetSpellDamage(int v) { data_.enchanted_states.spell_damage = v; }

			void IncreaseNumAttacksThisTurn() { ++data_.num_attacks_this_turn; }
			void ClearNumAttacksThisTurn() { data_.num_attacks_this_turn = 0; }

			void IncreaseUsedThisTurn() { ++data_.used_this_turn; }
			void ClearUsedThisTurn() { data_.used_this_turn = 0; }

			void SetUsable(bool v = true) { data_.usable = v; }

			void SetJustPlayedFlag(bool v) { data_.just_played = v; }

			auto& GetMutableEnchantmentHandler() { return data_.enchantment_handler; }
			auto& GetMutableDeathrattleHandler() { return data_.deathrattle_handler; }

			MutableAuraHandlerGetter GetMutableAuraHandlerGetter()
			{
				return MutableAuraHandlerGetter(data_);
			}
			MutableFlagAuraHandlerGetter GetMutableFlagAuraHandlerGetter()
			{
				return MutableFlagAuraHandlerGetter(data_);
			}

			ZoneSetter SetZone() { return ZoneSetter(data_); }
			ZonePosSetter SetZonePos() { return ZonePosSetter(data_); }

			void SetSilenced() { data_.silenced = true; }

			void BecomeCopyOf(CardData const& new_data) {
				auto old_player = data_.enchanted_states.player;
				auto old_zone = data_.zone;
				auto old_zone_position = data_.zone_position;
				
				data_ = new_data;

				data_.enchanted_states.player = old_player;
				data_.zone = old_zone;
				data_.zone_position = old_zone_position;
			}

		public:
			const CardData & GetRawData() const { return data_; }

		private:
			CardData data_;
		};
	}
}