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

	namespace onplay
	{
		namespace context
		{
			struct GetSpecifiedTarget {
			public:
				GetSpecifiedTarget(
					Manipulate & manipulate,
					state::PlayerIdentifier player,
					state::CardRef card_ref,
					bool * allow_no_target,
					bool * need_to_prepare_target) :
					manipulate_(manipulate),
					player_(player),
					card_ref_(card_ref),
					allow_no_target_(allow_no_target),
					need_to_prepare_target_(need_to_prepare_target)
				{}

			public:
				Manipulate & manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;

				// TODO: add helper functions to set jup targets_, allow_no_target, and need_to_prepare_target_

				template <typename... Args>
				state::targetor::TargetsGenerator & SetTargets(Args&&... args) {
					targets_generator_.Initialize(std::forward<Args>(args)...);
					return targets_generator_;
				}
				state::targetor::Targets GetTargets() {
					return targets_generator_.GetInfo();
				}

				bool * allow_no_target_; // default: allow
				bool * need_to_prepare_target_; // default: need // TODO: need this one? what's this for?

			private:
				state::targetor::TargetsGenerator targets_generator_;
			};

			struct OnPlay
			{
				Manipulate & manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;
				state::CardRef * new_card_ref;

				state::CardRef GetTarget() const;
			};
		}
	}
}