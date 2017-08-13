#pragma once

#include <string>
#include <unordered_map>
#include "state/Types.h"
#include "Cards/id-map.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Secrets
		{
		public:
			Secrets() : secrets_() {}

			Secrets(Secrets const* base) :
				secrets_(base->secrets_) // TODO: copy on write
			{}

			bool Exists(::Cards::CardId card_id) const
			{
				return secrets_.find((int)card_id) != secrets_.end();
			}

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				for (auto const& kv : secrets_) {
					functor(kv.second);
				}
			}

			bool Empty() const { return secrets_.empty(); }

			void Add(::Cards::CardId card_id, CardRef card)
			{
				if (Exists(card_id)) throw std::runtime_error("Secret already exists");
				secrets_.insert(std::make_pair((int)card_id, card));
			}

			void Remove(::Cards::CardId card_id)
			{
				secrets_.erase(card_id);
			}

			void Clear()
			{
				secrets_.clear();
			}

		private:
			std::unordered_map<int, CardRef> secrets_;
		};
	}
}