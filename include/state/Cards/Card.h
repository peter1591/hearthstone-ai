#pragma once

#include <string>
#include <utility>
#include "Cards/CardDispatcher.h"
#include "Cards/id-map.h"
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

		class CardManipulator;
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
			class ZoneSetter {
			public:
				ZoneSetter(CardData & data) : data_(data) {}
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
				friend class FlowControl::Manipulators::CardManipulator;
			public:
				DamageSetter(CardData & data) : data_(data) {}
			private:
				void Set(int v) { data_.damaged = v; }
			private:
				CardData & data_;
			};

		public:
			Card() : data_() {}
			explicit Card(const CardData & data) : data_(data) {}
			explicit Card(CardData&& data) : data_(std::move(data)) {}

			void RestoreToDefault() {
				auto data = ::Cards::CardDispatcher::CreateInstance(GetCardId());
				data.enchanted_states.player = data_.enchanted_states.player;
				data.zone = data_.zone;
				data.zone_position = data_.zone_position;

				data.enchantment_handler.SetOriginalStates(data.enchanted_states);
				data_ = data;
			}

		public: // getters and setters
			::Cards::CardId GetCardId() const { return data_.card_id; }
			CardType GetCardType() const { return data_.card_type; }
			bool IsSecretCard() const { return data_.is_secret_card; }
			CardRace GetRace() const { return data_.card_race; }

			int GetPlayOrder() const { return data_.play_order; }
			void SetPlayOrder(int play_order) { data_.play_order = play_order; }

			const PlayerIdentifier GetPlayerIdentifier() const { return data_.enchanted_states.player; }
			CardZone GetZone() const { return data_.zone; }
			int GetZonePosition() const { return data_.zone_position; }

			int GetCost() const { return data_.enchanted_states.cost; }
			void SetCost(int new_cost) { data_.enchanted_states.cost = new_cost; }

			int GetDamage() const { return data_.damaged; }
			DamageSetter GetDamageSetter() { return DamageSetter(data_); }

			int GetHP() const { return data_.enchanted_states.max_hp - data_.damaged; }

			int GetArmor() const { return data_.armor; }
			void SetArmor(int v) { data_.armor = v; }

			int GetAttack() const { return data_.enchanted_states.attack; }
			void SetAttack(int new_attack) { data_.enchanted_states.attack = new_attack; }

			int GetMaxHP() const { return data_.enchanted_states.max_hp; }
			void SetMaxHP(int max_hp) { data_.enchanted_states.max_hp = max_hp; }

			void SetTaunt(bool v) { data_.taunt = v; }
			bool HasTaunt() const { return data_.taunt; }

			void SetShield(bool v) { data_.shielded = v; }
			bool HasShield() const { return data_.shielded; }

			void SetCantAttack(bool v) { data_.cant_attack = v; }

			void SetFreezed(bool v) { data_.freezed = v; }

			void SetCharge(bool v) { data_.enchanted_states.charge = v; }
			bool HasCharge() const { return data_.enchanted_states.charge; }

			void SetStealth(bool v) { data_.enchanted_states.stealth = v; }
			bool HasStealth() const { return data_.enchanted_states.stealth; }

			void SetMaxAttacksPerTurn(int v) { data_.enchanted_states.max_attacks_per_turn = v; }
			int GetMaxAttacksPerTurn() const { return data_.enchanted_states.max_attacks_per_turn; }

			void SetImmuneToSpell(bool v) { data_.enchanted_states.immune_to_spell = v; }
			bool IsImmuneToSpell() const { return data_.enchanted_states.immune_to_spell; }

			void SetPoisonous(bool v) { data_.enchanted_states.poisonous = v; }
			bool IsPoisonous() const { return data_.enchanted_states.poisonous; }

			void SetFreezeAttack(bool v) { data_.enchanted_states.freeze_attack = v; }
			bool IsFreezeAttack() const { return data_.enchanted_states.freeze_attack; }

			void SetCantAttackHero(bool v) { data_.enchanted_states.cant_attack_hero = v; }
			bool IsCantAttackHero() const { return data_.enchanted_states.cant_attack_hero; }

			int GetSpellDamage() const { return data_.enchanted_states.spell_damage; }
			void SetSpellDamage(int v) { data_.enchanted_states.spell_damage = v; }

			void IncreaseNumAttacksThisTurn() { ++data_.num_attacks_this_turn; }
			void ClearNumAttacksThisTurn() { data_.num_attacks_this_turn = 0; }

			void IncreaseUsedThisTurn() { ++data_.used_this_turn; }
			void ClearUsedThisTurn() { data_.used_this_turn = 0; }

			void SetUsable(bool v = true) { data_.usable = v; }

			void SetJustPlayedFlag(bool v) { data_.just_played = v; }

			auto const& GetEnchantmentHandler() const { return data_.enchantment_handler; }
			auto& GetMutableEnchantmentHandler() { return data_.enchantment_handler; }
			auto& GetMutableDeathrattleHandler() { return data_.deathrattle_handler; }

			ZoneSetter SetZone() { return ZoneSetter(data_); }
			ZonePosSetter SetZonePos() { return ZonePosSetter(data_); }

			void SetSilenced() { data_.silenced = true;; }
			bool IsSilenced() const { return data_.silenced; }

			void SetPendingDestroy() { data_.pending_destroy = true; }
			bool GetPendingDestroy() const { return data_.pending_destroy; }

			void SetImmune(bool v) { data_.enchanted_states.immune = v; }
			bool GetImmune() const { return data_.enchanted_states.immune; }

		public:
			const CardData & GetRawData() const { return data_; }

		private:
			CardData data_;
		};
	}
}
