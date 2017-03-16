#pragma once

#include "FlowControl/battlecry/Contexts.h"

namespace FlowControl
{
	namespace battlecry
	{
		class Handler
		{
		public:
			typedef state::targetor::Targets BattlecryTargetGetter(context::BattlecryTargetGetter);
			typedef void BattlecryCallback(context::BattleCry);

			Handler() : battlecry_target_getter(nullptr), battlecry(nullptr) {}

			void SetCallback_Battlecry(BattlecryCallback* callback) { battlecry = callback; }
			void SetCallback_BattlecryTargetGetter(BattlecryTargetGetter* callback) { battlecry_target_getter = callback; }

		public:
			void PrepareBattlecryTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;
			void DoBattlecry(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;

		private:
			BattlecryTargetGetter *battlecry_target_getter;
			BattlecryCallback *battlecry;
		};
	}
}