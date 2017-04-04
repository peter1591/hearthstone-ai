#pragma once

#include <vector>
#include "state/Types.h"
#include "FlowControl/aura/Handler.h"

namespace state {
	namespace aura {
		class Manager
		{
		public:
			void Add(CardRef ref, FlowControl::aura::Handler handler) {
				auras_.push_back(std::make_pair(ref, std::move(handler)));
			}

			template <typename Functor> // Functor = bool(CardRef, FlowControl::aura::Handler &). returns false to remove it from container
			void ForEachAura(Functor&& functor) {
				for (auto it = auras_.begin(); it != auras_.end();) {
					if (!functor(it->first, it->second)) {
						it = auras_.erase(it);
					}
					else {
						++it;
					}
				}
			}

		private:
			std::vector<std::pair<CardRef, FlowControl::aura::Handler>> auras_;
		};
	}
}