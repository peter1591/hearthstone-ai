#pragma once

#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		namespace context
		{
			struct GetSpecifiedTarget;
			struct OnPlay;
		}

		class Handler
		{
		public:
			typedef void SpecifiedTargetGetter(context::GetSpecifiedTarget &);
			typedef void OnPlayCallback(context::OnPlay const&);

			Handler() : specified_target_getter(nullptr), onplay(nullptr) {}

			void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
			void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

		public:
			bool PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const;
			void OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const;

		private:
			SpecifiedTargetGetter *specified_target_getter;
			OnPlayCallback *onplay;
		};
	}
}