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
			typedef void DeathrattleCallback(context::Deathrattle);

			void Clear() { deathrattles.clear(); }

			void Add(DeathrattleCallback* deathrattle) {
				deathrattles.push_back(deathrattle);
			}

			void TriggerAll(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card);

			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles deathrattles;
		};
	}
}