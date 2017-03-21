#pragma once

#include "FlowControl/onplay/Contexts.h"

namespace FlowControl
{
	namespace onplay
	{
		class Handler
		{
		public:
			typedef state::targetor::Targets SpecifiedTargetGetter(context::GetSpecifiedTarget);
			typedef void OnPlayCallback(context::OnPlay);

			Handler() : specified_target_getter(nullptr), onplay(nullptr) {}

			void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
			void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

		public:
			void PrepareTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;
			void OnPlay(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card,
				state::CardRef * new_card_ref, state::Cards::Card const* * new_card) const;

		private:
			SpecifiedTargetGetter *specified_target_getter;
			OnPlayCallback *onplay;
		};
	}
}