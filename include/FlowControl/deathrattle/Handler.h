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
	class FlowContext;

	namespace deathrattle
	{
		namespace context
		{
			struct Deathrattle
			{
				state::State & state_;
				FlowContext & flow_context_;
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

			void TriggerAll(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card & card) {
				for (auto deathrattle : deathrattles) {
					deathrattle(context::Deathrattle{ state, flow_context, card_ref, card });
				}
			}

			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles deathrattles;
		};
	}
}