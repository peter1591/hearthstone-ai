#pragma once

#include "FlowControl/spell/Contexts.h"

namespace FlowControl
{
	namespace spell
	{
		class Handler
		{
		public:
			typedef state::targetor::Targets TargetGetter(context::TargetGetter);
			typedef void DoSpellCallback(context::DoSpell);

			Handler() : target_getter(nullptr), do_spell(nullptr) {}

			void SetCallback_TargetGetter(TargetGetter* callback) { target_getter = callback; }
			void SetCallback_DoSpell(DoSpellCallback* callback) { do_spell = callback; }

		public:
			void PrepareTarget(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;
			void DoSpell(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;

		private:
			TargetGetter *target_getter;
			DoSpellCallback *do_spell;
		};
	}
}