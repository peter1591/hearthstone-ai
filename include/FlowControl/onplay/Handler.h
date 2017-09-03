#pragma once

#include "FlowControl/FlowContext.h"

namespace FlowControl
{
	namespace onplay
	{
		namespace context
		{
			struct CheckPlayable;
			struct GetSpecifiedTarget;
			struct OnPlay;
		}

		class Handler
		{
		public:
			typedef bool CheckPlayableCallback(context::CheckPlayable &);
			typedef void SpecifiedTargetGetter(context::GetSpecifiedTarget &);
			typedef void OnPlayCallback(context::OnPlay const&);

			Handler() : check_playable(nullptr), specified_target_getter(nullptr), onplay(nullptr) {}

			void SetCheckPlayableCallback(CheckPlayableCallback* callback) { check_playable = callback; }
			void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
			void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

		public:
			bool CheckPlayable(state::State const& state, state::PlayerIdentifier player) const;
			bool PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const;
			void OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const;

		private:
			CheckPlayableCallback *check_playable;
			SpecifiedTargetGetter *specified_target_getter;
			OnPlayCallback *onplay;
		};
	}
}