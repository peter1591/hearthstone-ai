#pragma once

#include <functional>
#include <unordered_set>
#include "state/Types.h"
#include "state/Cards/Enchantments/Enchantments.h"

namespace state
{
	class State;
	class FlowContext;
	namespace Cards
	{
		class Card;
		namespace aura { class AuraAuxData; }
	}
}

namespace FlowControl
{
	namespace Context
	{
		struct AddedToPlayZone
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct AuraApplyOn
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::CardRef target_;
			state::Cards::Enchantments::ContainerType::Identifier & enchant_id_;
		};

		struct AuraGetTargets
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::Cards::aura::AuraAuxData & aura_data_;
			std::unordered_set<state::CardRef> & targets_;
		};

		struct AuraRemoveFrom
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			const state::CardRef card_ref_;
			const state::Cards::Card & card_;
			const state::CardRef target_;
			const state::Cards::Enchantments::ContainerType::Identifier enchant_id_;
		};

		struct BattleCry
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct BattlecryTargetGetter
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct Deathrattle
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct EnchantmentAfterAdded
		{
			state::State & state_;
			state::FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Enchantments::ContainerType::Identifier enchant_id_;
		};
	}
}