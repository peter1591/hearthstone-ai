#pragma once

#include <vector>
#include "state/Types.h"
#include "FlowControl/aura/Handler.h"

namespace state {
	namespace aura {
		class Manager
		{
		public:
			void Add(FlowControl::aura::Handler handler) {
				auras_.push_back(std::move(handler));
			}

			template <typename Functor> // Functor = bool(FlowControl::aura::Handler &). returns false to remove it from container
			void ForEachAura(Functor&& functor) {
				for (auto it = auras_.begin(); it != auras_.end();) {
					if (!functor(*it)) {
						it = auras_.erase(it);
					}
					else {
						++it;
					}
				}
			}

		private:
			std::vector<FlowControl::aura::Handler> auras_;
		};
	}
}