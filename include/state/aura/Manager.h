#pragma once

#include <vector>
#include "state/Types.h"

namespace state {
	namespace aura {
		class Manager
		{
		public:
			void Add(CardRef ref) { card_refs_.push_back(ref); }

			template <typename Functor> // Functor = bool(CardRef). returns false to remove it from container
			void ForEach(Functor&& functor) {
				for (auto it = card_refs_.begin(); it != card_refs_.end();) {
					if (!functor(*it)) {
						it = card_refs_.erase(it);
					}
					else {
						++it;
					}
				}
			}

		private:
			std::vector<CardRef> card_refs_;
		};
	}
}