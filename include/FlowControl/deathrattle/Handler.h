#pragma once

#include "state/Types.h"
#include "state/targetor/TargetsGenerator.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	namespace deathrattle
	{
		namespace context {
			struct Deathrattle;
		}

		class Handler
		{
		public:
			typedef void DeathrattleCallback(context::Deathrattle const&);

			Handler() : deathrattles_() {}

			void Clear() { deathrattles_.clear(); }

			void Add(DeathrattleCallback* deathrattle) {
				deathrattles_.push_back(deathrattle);
			}

			void TriggerAll(context::Deathrattle const& context) const {
				for (auto deathrattle : deathrattles_) {
					(*deathrattle)(context);
				}
			}

		private:
			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles deathrattles_;
		};
	}
}