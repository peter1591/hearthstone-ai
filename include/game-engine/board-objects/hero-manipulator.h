#pragma once

#include <stdexcept>

#include "hero.h"
#include "object-base.h"

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
			int GetHP() const { return this->hero.hp; }
			int GetAttack() const {
				throw std::runtime_error("not yet implemented");
			}

			void TakeDamage(int damage, bool poisonous) {
				// Note: poisonous have no effect on heros
				this->hero.hp -= damage;
			}

			void AttackedOnce() {
				// TODO
			}

			bool IsForgetful() const { 
				// TODO
				throw std::runtime_error("not yet implemented");
			}

			void SetFreezeAttacker(bool freeze) {
				throw std::runtime_error("not yet implemented");
			}

			bool IsFreezeAttacker() const {
				// TODO
				return false;
			}

			void SetFreezed(bool freezed) {
				// TODO
				throw std::runtime_error("not yet implemented");
			}

			bool IsFreezed() const {
				// TODO
				return false;
			}

			bool IsPoisonous() const {
				// TODO
				return false;
			}

		private:
			Board & board;
			Hero & hero;
		};

	} // BoardObjects
} // GameEngine