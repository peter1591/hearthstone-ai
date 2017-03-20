#pragma once

#include "FlowControl/battlecry/Contexts.h"

namespace FlowControl
{
	namespace battlecry
	{
		class Handler
		{
		public:
			typedef void BattlecryCallback(context::BattleCry);

			Handler() : battlecry(nullptr) {}

			void SetCallback_Battlecry(BattlecryCallback* callback) { battlecry = callback; }

		public:
			void DoBattlecry(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;

		private:
			BattlecryCallback *battlecry;
		};
	}
}