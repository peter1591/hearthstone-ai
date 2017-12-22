#pragma once

namespace Cards
{
	struct Card_CFM_316t: MinionCardBase<Card_CFM_316t> {};
	struct Card_CFM_316 : MinionCardBase<Card_CFM_316> {
		Card_CFM_316() {
			this->deathrattle_handler.Add([](engine::FlowControl::deathrattle::context::Deathrattle const& context) {
				for (int i = 0; i < context.attack_; ++i) {
					SummonAt(context, context.player_, context.zone_pos_, CardId::ID_CFM_316t);
				}
			});
		}
	};
}

REGISTER_CARD(CFM_316t)
REGISTER_CARD(CFM_316)
