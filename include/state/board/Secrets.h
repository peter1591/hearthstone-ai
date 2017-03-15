#pragma once

#include <string>
#include <unordered_map>
#include "state/Types.h"

namespace state
{
	namespace detail { template <CardType TargetCardType, CardZone TargetCardZone> struct PlayerDataStructureMaintainer; }

	namespace board
	{
		class Secrets
		{
			template <CardType TargetCardType, CardZone TargetCardZone> friend struct state::detail::PlayerDataStructureMaintainer;

			bool Exists(int card_id) const
			{
				return secrets_.find(card_id) != secrets_.end();
			}

		private:
			void Add(int card_id, CardRef card)
			{
				if (Exists(card_id)) throw std::exception("Secret already exists");
				secrets_.insert(std::make_pair(card_id, card));
			}

			void Remove(int card_id)
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