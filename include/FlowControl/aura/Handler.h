#pragma once

#include <unordered_map>
#include "FlowControl/enchantment/TieredEnchantments.h"
#include "FlowControl/aura/Contexts.h"

namespace FlowControl
{
	namespace aura
	{
		class Handler
		{
		public:
			typedef bool FuncIsValid(contexts::AuraIsValid context);
			typedef void FuncGetTargets(contexts::AuraGetTargets context);
			typedef void FuncApplyOn(contexts::AuraApplyOn context); // TODO: return enchantment id
			typedef void FuncRemoveFrom(contexts::AuraRemoveFrom context);

			Handler() :
				is_valid(nullptr), get_targets(nullptr), apply_on(nullptr), remove_from(nullptr),
				last_updated_change_id_first_player_minions_(-1), // ensure this is not the initial value of the actual change id
				last_updated_change_id_second_player_minions_(-1)
			{}

			bool NoAppliedEnchantment() const { return applied_enchantments.empty(); }

			bool Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card);

		public: // TODO: encapsulate
			FuncIsValid * is_valid;
			FuncGetTargets * get_targets;
			FuncApplyOn * apply_on;
			FuncRemoveFrom * remove_from;

			std::unordered_map<state::CardRef, enchantment::TieredEnchantments::ContainerType::Identifier> applied_enchantments;
			int last_updated_change_id_first_player_minions_;
			int last_updated_change_id_second_player_minions_;
		};
	}
}