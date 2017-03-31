#pragma once

namespace Cards
{
	struct Card_EX1_531 : MinionCardBase<Card_EX1_531> {};
	struct Card_EX1_534 : MinionCardBase<Card_EX1_534> {
		Card_EX1_534() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle const& context) {
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_531);
				SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_531);
			});
		}
	};
}

REGISTER_CARD(EX1_531)
REGISTER_CARD(EX1_534)
