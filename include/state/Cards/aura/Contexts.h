#pragma once

#include "state/Cards/Enchantments/Enchantments.h"

namespace FlowControl { class FlowContext; }

namespace state {
	class State;
	class CardRef;

	namespace targetor { class TargetsGenerator; }

	namespace Cards {
		class Card;

		namespace aura {
			namespace contexts {
				struct AuraApplyOn
				{
					State & state_;
					FlowControl::FlowContext & flow_context_;
					CardRef card_ref_;
					const Card & card_;
					CardRef target_;
					Enchantments::ContainerType::Identifier & enchant_id_;
				};

				struct AuraGetTargets
				{
					State & state_;
					FlowControl::FlowContext & flow_context_;
					CardRef card_ref_;
					const Card & card_;
					aura::AuraAuxData & aura_data_;
					targetor::TargetsGenerator & targets_generator_;
				};

				struct AuraIsValid
				{
					State & state_;
					FlowControl::FlowContext & flow_context_;
					CardRef card_ref_;
					const Card & card_;
				};

				struct AuraRemoveFrom
				{
					State & state_;
					FlowControl::FlowContext & flow_context_;
					const CardRef card_ref_;
					const Card & card_;
					const CardRef target_;
					const Enchantments::ContainerType::Identifier enchant_id_;
				};
			}
		}
	}
}