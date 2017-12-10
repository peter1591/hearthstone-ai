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
			class ContainerType
			{
			public:
				ContainerType() : base_(nullptr), items_() {}
				ContainerType(ContainerType const& rhs) : base_(rhs.base_), items_(rhs.items_) {}
				ContainerType & operator=(ContainerType const& rhs) {
					base_ = rhs.base_;
					items_ = rhs.items_;
					return *this;
				}

				void FillWithBase(ContainerType const& base) {
					assert(base.base_ == nullptr);
					base_ = &base.items_;
				}

				template <class... Args>
				auto push_back(Args&&... args) {
					GetContainerForWrite().push_back(std::forward<Args>(args)...);
				}

				auto begin() {
					return GetContainerForWrite().begin();
				}

				auto end() {
					return GetContainerForWrite().end();
				}

				template <class Arg>
				auto erase(Arg&& arg) {
					return GetContainerForWrite().erase(std::forward<Arg>(arg));
				}

				auto size() const {
					return GetContainerForRead().size();
				}

			private:
				using InternalContainerType = std::vector<CardRef>;

				InternalContainerType const& GetContainerForRead() const {
					if (base_) return *base_;
					else return items_;
				}

				InternalContainerType & GetContainerForWrite() {
					if (base_) {
						items_ = *base_; // copy on write
						base_ = nullptr;
					}
					return items_;
				}

			private:
				InternalContainerType const* base_;
				InternalContainerType items_;
			};

		public:
			Graveyard() : minions_(), spells_(), others_() {}

			void FillWithBase(Graveyard const& base) {
				minions_.FillWithBase(base.minions_);
				spells_.FillWithBase(base.spells_);
				others_.FillWithBase(base.others_);
			}

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