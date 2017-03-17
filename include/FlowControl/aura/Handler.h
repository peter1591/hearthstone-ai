#pragma once

#include <unordered_map>
#include "state/targetor/Targets.h"
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
			typedef enchantment::TieredEnchantments::IdentifierType FuncApplyOn(contexts::AuraApplyOn context);
			typedef void FuncRemoveFrom(contexts::AuraRemoveFrom context);

			Handler() :
				is_valid(nullptr), get_targets(nullptr), apply_on(nullptr),
				first_time_update_(true),
				last_updated_change_id_first_player_minions_(-1), // ensure this is not the initial value of the actual change id
				last_updated_change_id_second_player_minions_(-1),
				last_updated_undamaged_(true)
			{}

		public:
			void SetCallback_IsValid(FuncIsValid* callback) { is_valid = callback; }
			void SetCallback_GetTargets(FuncGetTargets* callback) { get_targets = callback; }
			void SetCallback_ApplyOn(FuncApplyOn* callback) { apply_on = callback; }

			bool IsCallbackSet_IsValid() const { return is_valid != nullptr; }
			bool IsCallbackSet_GetTargets() const { return get_targets != nullptr; }
			bool IsCallbackSet_ApplyOn() const { return apply_on != nullptr; }

		public:
			bool NoAppliedEnchantment() const { return applied_enchantments.empty(); }
			bool Update(state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card);

		private:
			void GetNewTargets(
				state::State & state, FlowControl::FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card,
				bool* aura_valid, bool* need_update, std::unordered_set<state::CardRef>* new_targets);

		public: // field for client code
			bool first_time_update_;
			int last_updated_change_id_first_player_minions_;
			int last_updated_change_id_second_player_minions_;
			bool last_updated_undamaged_;

		private:
			FuncIsValid * is_valid;
			FuncGetTargets * get_targets;
			FuncApplyOn * apply_on;

			std::unordered_map<state::CardRef, enchantment::TieredEnchantments::IdentifierType> applied_enchantments;
		};
	}
}