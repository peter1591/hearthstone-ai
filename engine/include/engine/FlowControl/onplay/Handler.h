#pragma once

#include "engine/FlowControl/FlowContext.h"

namespace engine {
	namespace FlowControl
	{
		namespace onplay
		{
			namespace context
			{
				struct CheckPlayable;
				struct GetChooseOneOptions;
				struct GetSpecifiedTarget;
				struct OnPlay;
			}

			class Handler
			{
			public:
				typedef bool CheckPlayableCallback(context::CheckPlayable &);
				typedef Cards::CardId GetChooseOneOptionsCallback(context::GetChooseOneOptions &);
				typedef void SpecifiedTargetGetter(context::GetSpecifiedTarget &);
				typedef void OnPlayCallback(context::OnPlay const&);

				Handler() : check_playable(nullptr), choose_one(nullptr), specified_target_getter(nullptr), onplay(nullptr) {}

				void SetCheckPlayableCallback(CheckPlayableCallback* callback) { check_playable = callback; }
				void SetChooseOneOptionsCallback(GetChooseOneOptionsCallback * callback) { choose_one = callback; }
				void SetOnPlayCallback(OnPlayCallback* callback) { onplay = callback; }
				void SetSpecifyTargetCallback(SpecifiedTargetGetter* callback) { specified_target_getter = callback; }

			public:
				bool CheckPlayable(state::State const& state, state::PlayerIdentifier player, state::CardRef card_ref) const;
				void PrepareTarget(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref) const;
				void OnPlay(state::State & state, FlowContext & flow_context, state::PlayerIdentifier player, state::CardRef card_ref, state::CardRef * new_card_ref) const;

			private:
				void PrepareChooseOne(state::State & state, FlowContext & flow_context) const;

			private:
				CheckPlayableCallback *check_playable;
				GetChooseOneOptionsCallback *choose_one;
				SpecifiedTargetGetter *specified_target_getter;
				OnPlayCallback *onplay;
			};
		}
	}
}