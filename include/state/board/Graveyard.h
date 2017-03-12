#pragma once

#include <assert.h>
#include <unordered_set>
#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/Cards/Card.h"

namespace state
{
	namespace board
	{
		class Graveyard
		{
		public:
			size_t GetTotalMinions() const { return minions_.size(); }
			size_t GetTotalSpells() const { return spells_.size(); }
			size_t GetTotalOthers() const { return others_.size(); }

		public:
			template <CardType AddingType> void Add(CardRef ref) { AddInternal(ref, others_); }
			template <> void Add<kCardTypeMinion>(CardRef ref) { AddInternal(ref, minions_); }
			template <> void Add<kCardTypeSpell>(CardRef ref) { AddInternal(ref, spells_); }

		public:
			template <CardType RemovingType> void Remove(CardRef ref) { RemoveInternal(ref, others_); }
			template <> void Remove<kCardTypeMinion>(CardRef ref) { RemoveInternal(ref, minions_); }
			template <> void Remove<kCardTypeSpell>(CardRef ref) { RemoveInternal(ref, spells_); }

		private:
			void AddInternal(CardRef ref, std::unordered_set<CardRef>& container)
			{
				auto success = container.insert(ref).second;
				assert(success);
			}

			void RemoveInternal(CardRef ref, std::unordered_set<CardRef>& container)
			{
				auto ret = container.erase(ref);
				assert(ret == 1);
			}

		private:
			std::unordered_set<CardRef> minions_;
			std::unordered_set<CardRef> spells_;
			std::unordered_set<CardRef> others_;
		};
	}
}