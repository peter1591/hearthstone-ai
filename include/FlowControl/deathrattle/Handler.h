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
	class Manipulate;

	namespace deathrattle
	{
		namespace context
		{
			struct Deathrattle
			{
				Manipulate & manipulate_;
				state::CardRef card_ref_;
				const state::Cards::Card & card_;
			};
		}

		class Handler
		{
		public:
			typedef void DeathrattleCallback(context::Deathrattle const&);

			void Clear() { deathrattles.clear(); }

			void Add(DeathrattleCallback* deathrattle) {
				deathrattles.push_back(deathrattle);
			}

			void TriggerAll(context::Deathrattle const& context) {
				for (auto deathrattle : deathrattles) {
					(*deathrattle)(context);
				}
			}

			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles deathrattles;
		};
	}
}