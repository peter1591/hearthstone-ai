#pragma once

#include "FlowControl/onplay/Contexts.h"

namespace FlowControl
{
	namespace onplay
	{
		class Handler
		{
		public:
			typedef state::targetor::Targets SpecifiedTargetGetter(context::GetSpecifiedTarget const&);
			typedef bool OnPlayCallback(context::OnPlay const&); // return false if card cannot be played

			Handler() : specified_target_getter(nullptr), onplay(nullptr) {}

			void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
			void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

		public:
			bool PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const;
			bool OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const;

		private:
			SpecifiedTargetGetter *specified_target_getter;
			OnPlayCallback *onplay;
		};
	}
}