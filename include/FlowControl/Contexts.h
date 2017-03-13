#pragma once

#include <functional>
#include <unordered_set>
#include "state/Types.h"
#include "state/Cards/Enchantments/Enchantments.h"

namespace state
{
	class State;
	namespace Cards
	{
		class Card;
		namespace aura { class AuraAuxData; }
	}
}

namespace FlowControl
{
	class FlowContext;
	namespace Context
	{
		struct AddedToPlayZone
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct BattleCry
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct BattlecryTargetGetter
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		/* TODO
		struct BattleycryGenerateTargets
		{
			state::State & state_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
			state::utils::Targets & 
		};*/

		struct Deathrattle
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			const state::Cards::Card & card_;
		};

		struct EnchantmentAfterAdded
		{
			state::State & state_;
			FlowContext & flow_context_;
			state::CardRef card_ref_;
			state::Cards::Enchantments::ContainerType::Identifier enchant_id_;
		};
	}
}