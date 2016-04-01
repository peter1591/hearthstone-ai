#pragma once

#include <stdexcept>

#include "hero-data.h"
#include "object-base.h"
#include "enchantments.h"

namespace GameEngine {
	class Board;

	namespace BoardObjects {

		class Hero : public ObjectBase
		{
			friend std::hash<Hero>;

		public:
			typedef Weapon::OnDeathTrigger WeaponOnDeathTrigger;

		public:
			Hero(Board & board) : board(board) {}
			Hero(Board & board, Hero const& rhs) : board(board), hero(rhs.hero) {}
			Hero & operator=(Hero const& rhs) = delete;
			Hero(Board & board, Hero && rhs) : board(board), hero(std::move(rhs.hero)) {}
			Hero & operator=(Hero && rhs) {
				this->hero = std::move(rhs.hero);
				return *this;
			}

			bool operator==(Hero const& rhs) const { return this->hero == rhs.hero; }
			bool operator!=(Hero const& rhs) const { return this->hero != rhs.hero; }

			Board & GetBoard() const { return this->board; }

			void SetHero(HeroData const& hero) { this->hero = hero; }

			std::string GetDebugString() const { return this->hero.GetDebugString(); }

		public:
			int GetHP() const { 
				return this->hero.hp; 
			}

			int GetAttack() const 
			{
				int attack = this->GetWeaponAttack();
				// TODO: hero can have its attack value in addition to the weapon
				return attack;
			}

			int GetWeaponAttack() const
			{
				if (!this->hero.weapon.IsValid()) return 0;
				return this->hero.weapon.attack;
			}

			bool Attackable() const
			{
				if (this->GetAttack() <= 0) return false;

				if (this->hero.freezed) return false;

				int max_attacked_times = 1;
				if (this->hero.weapon.windfury) max_attacked_times = 2;

				if (this->hero.attacked_times >= max_attacked_times) return false;

				return true;
			}

			void TakeDamage(int damage, bool poisonous) {
				// Note: poisonous have no effect on heros
				this->hero.hp -= damage;
			}

			void AttackedOnce() {
				if (this->hero.weapon.IsValid()) {
					--this->hero.weapon.durability;

					if (this->hero.weapon.durability <= 0) {
						// destroy weapon
						this->DestroyWeapon();
					}
				}
				++this->hero.attacked_times;
			}

			int GetForgetfulCount() const {
				return this->hero.weapon.forgetful;
			}

			bool IsFreezeAttacker() const {
				return this->hero.weapon.freeze_attack;
			}

			void SetFreezed() {
				this->hero.freezed = true;
			}

			void ClearFreezed() {
				this->hero.freezed = false;
			}

			bool IsFreezed() const {
				return this->hero.freezed;
			}

			bool IsPoisonous() const {
				// There's currently no way to make hero attack to be poisonous
				return false;
			}

			void AddWeaponOnDeathTrigger(WeaponOnDeathTrigger && func) {
				this->hero.weapon.on_death_triggers.push_back(std::move(func));
			}

		public: // hooks
			void TurnStart(bool owner_turn)
			{
				this->hero.attacked_times = 0;
			}

			void TurnEnd(bool owner_turn)
			{
				if (owner_turn) {
					// check thaw
					if (this->hero.attacked_times == 0 && this->IsFreezed()) {
						this->ClearFreezed();
					}
				}
			}

			void HookAfterMinionAdded(Minion & added_minion) {
			}

			void HookAfterMinionDamaged(Minions & minions, Minion & minion, int damage) {

			}


			void DestroyWeapon()
			{
				auto triggers = this->GetAndClearWeaponOnDeathTriggers();

				for (auto const& trigger : triggers) {
					trigger.func(*this);
				}

				this->hero.weapon.InValidate();
			}

			void EquipWeapon(Card const& card)
			{
#ifdef DEBUG
				if (card.type != Card::TYPE_WEAPON) throw std::runtime_error("invalid argument");
#endif
				this->hero.weapon.Clear();

				this->hero.weapon.card_id = card.id;
				this->hero.weapon.cost = card.cost;
				this->hero.weapon.attack = card.data.weapon.attack;
				this->hero.weapon.durability = card.data.weapon.durability;
				if (card.data.weapon.forgetful) this->hero.weapon.forgetful++;
				this->hero.weapon.freeze_attack = card.data.weapon.freeze;
				this->hero.weapon.windfury = card.data.weapon.windfury;
			}

		private:
			std::list<WeaponOnDeathTrigger> GetAndClearWeaponOnDeathTriggers() {
				std::list<WeaponOnDeathTrigger> ret;
				this->hero.weapon.on_death_triggers.swap(ret);
				return ret;
			}

		private:
			Board & board;
			HeroData hero;
		};

	} // BoardObjects
} // GameEngine

namespace std {
	template <> struct hash<GameEngine::BoardObjects::Hero> {
		typedef GameEngine::BoardObjects::Hero argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<decltype(s.hero)>()(s.hero);
		}
	};
}