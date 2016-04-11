#pragma once

#include <stdexcept>

#include "hero-data.h"
#include "object-base.h"
#include "game-engine/enchantments/enchantments.h"

namespace GameEngine {
	class Board;

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
		HeroData & GetHeroDataForBoardInitialization() { return this->hero; }

		void SetHero(HeroData const& hero_)
		{
			this->hero = hero_;
#ifdef DEBUG
			if (!this->hero.hero_power.IsValid()) throw std::runtime_error("invalid hero power card");
#endif
		}

		std::string GetDebugString() const { return this->hero.GetDebugString(); }

	public:
		int GetHP() const {
			return this->hero.hp;
		}

		int GetAttack() const // Hero Attack + Weapon Attack
		{
			int attack = this->hero.attack + this->GetWeaponAttack();
			return attack;
		}

		int GetHeroAttack() const { return this->hero.attack; }
		void SetHeroAttack(int hero_attack) { this->hero.attack = hero_attack; }

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
			if (this->hero.weapon.IsValid() && this->hero.weapon.windfury) max_attacked_times = 2;

			if (this->hero.attacked_times >= max_attacked_times) return false;

			return true;
		}

		void TakeDamage(int damage, bool poisonous) {
			// Note: poisonous have no effect on heros
			(void)poisonous;
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
			if (!this->hero.weapon.IsValid()) return 0;
			return this->hero.weapon.forgetful;
		}

		bool IsFreezeAttacker() const {
			if (!this->hero.weapon.IsValid()) return false;
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
			if (!this->hero.weapon.IsValid()) throw std::runtime_error("no valid weapon");
			this->hero.weapon.on_death_triggers.push_back(std::move(func));
		}

		HeroPower const& GetHeroPower() const { return this->hero.hero_power; }
		void UsedHeroPowerOnce() { this->hero.hero_power.used_this_turn = true; }

		void Heal(int amount)
		{
			int new_hp = this->hero.hp + amount;
			if (new_hp > this->hero.max_hp) new_hp = this->hero.max_hp;
			this->hero.hp = new_hp;

			// TODO: trigger hooks
		}

		void GainArmor(int amount) {
			this->hero.armor += amount;
		}

	public: // hooks
		void TurnStart(bool owner_turn)
		{
			(void)owner_turn;

			this->hero.attacked_times = 0;
			this->hero.hero_power.used_this_turn = false;
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

		void HookAfterMinionAdded(Minion & added_minion)
		{
			(void)added_minion;
		}

		void HookAfterMinionDamaged(Minion & minion, int damage)
		{
			(void)minion;
			(void)damage;
		}

		void HookBeforeMinionTransform(Minion & minion, int new_card_id)
		{
			(void)minion;
			(void)new_card_id;
		}

		void HookAfterMinionTransformed(Minion & minion)
		{
			(void)minion;
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

} // GameEngine

namespace std {
	template <> struct hash<GameEngine::Hero> {
		typedef GameEngine::Hero argument_type;
		typedef std::size_t result_type;
		result_type operator()(const argument_type &s) const {
			return std::hash<decltype(s.hero)>()(s.hero);
		}
	};
}