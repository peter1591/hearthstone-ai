#pragma once

#include <stdexcept>

#include "hero.h"
#include "object-base.h"
#include "enchantments.h"

namespace GameEngine {
	class Board;

	namespace BoardObjects {

		class HeroManipulator : public ObjectBase
		{
		public:
			HeroManipulator(Board & board, Hero & hero)
				: board(board), hero(hero)
			{}

			Board & GetBoard() const { return this->board; }
			Hero const& GetHero() const { return this->hero; }

		public:
			int GetHP() const { 
				return this->hero.hp; 
			}

			int GetAttack() const { return this->hero.GetAttack(); }

			void TakeDamage(int damage, bool poisonous) const {
				// Note: poisonous have no effect on heros
				this->hero.hp -= damage;
			}

			void AttackedOnce() const {
				if (this->hero.weapon.IsVaild()) {
					--this->hero.weapon.durability;

					if (this->hero.weapon.durability <= 0) {
						// destroy weapon
						this->hero.weapon.InValidate();
					}
				}
				++this->hero.attacked_times;
			}

			bool IsForgetful() const {
				return this->hero.weapon.forgetful;
			}

			bool IsFreezeAttacker() const {
				return this->hero.weapon.freeze_attack;
			}

			void SetFreezed(bool freezed) const {
				this->hero.freezed = freezed;
			}

			bool IsFreezed() const {
				return this->hero.freezed;
			}

			bool IsPoisonous() const {
				// There's currently no way to make hero attack to be poisonous
				return false;
			}

		public: // hooks
			void TurnStart(bool owner_turn) const
			{
				this->hero.attacked_times = 0;
			}

			void TurnEnd(bool owner_turn) const
			{
				if (owner_turn) {
					// check thaw
					if (this->hero.attacked_times == 0 && this->IsFreezed()) {
						this->SetFreezed(false);
					}
				}
			}

			void DestroyWeapon() const
			{
				// TODO: trigger weapon deathrattle
				this->hero.weapon.InValidate();
			}

			void EquipWeapon(Card const& card) const
			{
#ifdef DEBUG
				if (card.type != Card::TYPE_WEAPON) throw std::runtime_error("invalid argument");
#endif

				this->hero.weapon.card_id = card.id;
				this->hero.weapon.cost = card.cost;
				this->hero.weapon.attack = card.data.weapon.attack;
				this->hero.weapon.durability = card.data.weapon.durability;
				this->hero.weapon.forgetful = card.data.weapon.forgetful;
				this->hero.weapon.freeze_attack = card.data.weapon.freeze;
				this->hero.weapon.windfury = card.data.weapon.windfury;
			}

		private:
			Board & board;
			Hero & hero;
		};

	} // BoardObjects
} // GameEngine