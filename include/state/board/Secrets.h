#pragma once

#include <string>
#include <unordered_map>
#include "state/Types.h"
#include "state/Cards/Manager.h"
#include "state/Cards/Card.h"

namespace state
{
	namespace board
	{
		class Secrets
		{
		public:
			void Add(int card_id, CardRef card)
			{
				if (Exists(card_id)) throw std::exception("Secret already exists");
				secrets_.insert(std::make_pair(card_id, card));
			}

			bool Exists(int card_id) const
			{
				return secrets_.find(card_id) != secrets_.end();
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