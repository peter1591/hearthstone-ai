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

		private:
			using ContainerType = std::vector<CardRef>;

		public:
			Graveyard() : minions_(), spells_(), others_() {}

			Graveyard(Graveyard const* base) :
				minions_(base->minions_), // TODO: copy-on-write
				spells_(base->spells_), // TODO: copy-on-write
				others_(base->others_) // TODO: copy-on-write
			{}

			size_t GetTotalMinions() const { return minions_.size(); }
			size_t GetTotalSpells() const { return spells_.size(); }
			size_t GetTotalOthers() const { return others_.size(); }

		private:
			template <CardType AddingType> void Add(CardRef ref);
			template <CardType RemovingType> void Remove(CardRef ref);

		private:
			void AddInternal(CardRef ref, ContainerType & container)
			{
				container.push_back(ref);
			}

			void RemoveInternal(CardRef ref, ContainerType & container)
			{
				for (auto it = container.begin(); it != container.end(); ) {
					if (*it == ref) {
						it = container.erase(it);
						break; // should only have one
					}
					else ++it;
				}
			}

		private:
			ContainerType minions_;
			ContainerType spells_;
			ContainerType others_;
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