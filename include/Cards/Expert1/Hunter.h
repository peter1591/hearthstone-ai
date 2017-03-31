#pragma once

namespace Cards
{
	struct Card_EX1_531 : MinionCardBase<Card_EX1_531> {};
	struct Card_EX1_534 : MinionCardBase<Card_EX1_534> {
		Card_EX1_534() {
			this->deathrattle_handler.Add([](FlowControl::deathrattle::context::Deathrattle&& context) {
				SummonToLeft(context, Cards::ID_EX1_531);
				SummonToLeft(context, Cards::ID_EX1_531);
			});
		}
	};
}

REGISTER_CARD(EX1_531)
REGISTER_CARD(EX1_534)
