#pragma once

#include "FlowControl/spell/Contexts.h"

namespace FlowControl
{
	namespace spell
	{
		class Handler
		{
		public:
			typedef void DoSpellCallback(context::DoSpell);

			Handler() : do_spell(nullptr) {}

			void SetCallback_DoSpell(DoSpellCallback* callback) { do_spell = callback; }

		public:
			void DoSpell(state::State & state, FlowContext & flow_context, state::CardRef card_ref, state::Cards::Card const& card) const;

		private:
			DoSpellCallback *do_spell;
		};
	}
}