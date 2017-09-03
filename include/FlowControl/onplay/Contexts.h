#pragma once

#include "state/Types.h"
#include "state/targetor/TargetsGenerator.h"
#include "FlowControl/Manipulate.h"

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
			struct CheckPlayable {
				CheckPlayable(
					state::State const& state,
					state::PlayerIdentifier player
				) :
					state_(state),
					player_(player)
				{}

				state::State const& state_;
				state::PlayerIdentifier player_;
			};

			struct GetSpecifiedTarget {
			public:
				GetSpecifiedTarget(
					state::State & state,
					FlowContext & flow_context,
					state::PlayerIdentifier player,
					state::CardRef card_ref) :
					manipulate_(state, flow_context),
					player_(player),
					card_ref_(card_ref),
					targets_generator_(),
					need_to_prepare_target_(false),
					allow_no_target_(false)
				{
				}

			public:
				Manipulate manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;

				template <typename... Args>
				auto & SetRequiredBattlecryTargets(Args&&... args) {
					return SetRequiredTargets(std::forward<Args>(args)...).Targetable();
				}
				template <typename... Args>
				auto & SetOptionalBattlecryTargets(Args&&... args) {
					return SetOptionalTargets(std::forward<Args>(args)...).Targetable();
				}

				template <typename... Args>
				auto & SetRequiredSpellTargets(Args&&... args) {
					// TODO: should check in CheckPlayable callback
					return SetRequiredTargets(std::forward<Args>(args)...).SpellTargetable();
				}
				template <typename... Args>
				auto & SetOptionalSpellTargets(Args&&... args) {
					return SetOptionalTargets(std::forward<Args>(args)...).SpellTargetable();
				}

				state::targetor::Targets GetTargets() {
					return targets_generator_.GetInfo();
				}

				bool NeedToPrepareTarget() { return need_to_prepare_target_; }
				bool IsAllowedNoTarget() { return allow_no_target_; }

			private:
				template <typename... Args>
				state::targetor::TargetsGenerator & SetRequiredTargets(Args&&... args) {
					need_to_prepare_target_ = true;
					allow_no_target_ = false;
					targets_generator_.Initialize(std::forward<Args>(args)...);
					return targets_generator_;
				}
				template <typename... Args>
				state::targetor::TargetsGenerator & SetOptionalTargets(Args&&... args) {
					need_to_prepare_target_ = true;
					allow_no_target_ = true;
					targets_generator_.Initialize(std::forward<Args>(args)...);
					return targets_generator_;
				}

			private:
				state::targetor::TargetsGenerator targets_generator_;
				bool need_to_prepare_target_;
				bool allow_no_target_;
			};

			struct OnPlay
			{
				Manipulate manipulate_;
				state::PlayerIdentifier player_;
				state::CardRef card_ref_;
				state::CardRef * new_card_ref;

				state::CardRef GetTarget() const;
			};
		}
	}
}