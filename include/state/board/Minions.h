#pragma once

#include <vector>
#include "state/Types.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Minions
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

		public:
			Minions() : change_id_(0)
			{
				minions_.reserve(max_size_);
			}

			size_t Size() const { return minions_.size(); }
			CardRef Get(size_t pos) const {
				assert(pos >= 0);
				assert(pos <= minions_.size());
				return minions_[pos];
			}
			void Replace(size_t pos, CardRef new_card_ref) {
				minions_[pos] = new_card_ref;
				++change_id_;
			}
			bool Full() const { return Size() >= max_size_; }

			std::vector<CardRef> const& Get() const { return minions_; }
			int GetChangeId() const { return change_id_; }
			void IncreaseChangeId() { ++change_id_; }

			template <typename Functor>
			void ForEach(Functor&& functor) {
				std::for_each(minions_.begin(), minions_.end(), std::forward<Functor>(functor));
			}

		private:
			template <typename AdjustFunctor>
			void Insert(CardRef ref, size_t pos, AdjustFunctor&& functor)
			{
				assert(pos >= 0);
				assert(pos <= minions_.size());
				assert(!Full());

				++change_id_;

				auto it = minions_.insert(minions_.begin() + pos, ref);
				functor(*it, pos);

				++it; ++pos;
				for (auto it_end = minions_.end(); it != it_end; ++it, ++pos) {
					functor(*it, pos);
				}
			}

			template <typename AdjustFunctor>
			void Remove(size_t pos, AdjustFunctor&& functor)
			{
				assert(pos < minions_.size());

				++change_id_;

				auto it = minions_.erase(minions_.begin() + pos);
				for (auto it_end = minions_.end(); it != it_end; ++it, ++pos) {
					functor(*it, pos);
				}
			}

		private:
			static constexpr int max_size_ = 7;
			std::vector<CardRef> minions_; // TODO: use std::array?
			int change_id_;
		};
	}
}