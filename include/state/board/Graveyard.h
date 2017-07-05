#pragma once

#include <assert.h>
#include <unordered_set>
#include "state/Types.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Graveyard
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

		public:
			size_t GetTotalMinions() const { return minions_.size(); }
			size_t GetTotalSpells() const { return spells_.size(); }
			size_t GetTotalOthers() const { return others_.size(); }

		private:
			template <CardType AddingType> void Add(CardRef ref);
			template <CardType RemovingType> void Remove(CardRef ref);

		private:
			void AddInternal(CardRef ref, std::unordered_set<CardRef>& container)
			{
				auto success = container.insert(ref).second;
				(void)success;
				assert(success);
			}

			void RemoveInternal(CardRef ref, std::unordered_set<CardRef>& container)
			{
				auto ret = container.erase(ref);
				(void)ret;
				assert(ret == 1);
			}

		private:
			std::unordered_set<CardRef> minions_;
			std::unordered_set<CardRef> spells_;
			std::unordered_set<CardRef> others_;
		};

		template<CardType AddingType>
		inline void Graveyard::Add(CardRef ref) { AddInternal(ref, others_); }
		template<CardType RemovingType>
		inline void Graveyard::Remove(CardRef ref) { RemoveInternal(ref, others_); }

		template<>
		inline void Graveyard::Add<kCardTypeMinion>(CardRef ref) { AddInternal(ref, minions_); }
		template<>
		inline void Graveyard::Remove<kCardTypeMinion>(CardRef ref) { RemoveInternal(ref, minions_); }

		template<>
		inline void Graveyard::Add<kCardTypeSpell>(CardRef ref) { AddInternal(ref, spells_); }
		template<>
		inline void Graveyard::Remove<kCardTypeSpell>(CardRef ref) { RemoveInternal(ref, spells_); }
	}
}