#pragma once

namespace Cards
{
	struct Card_OG_121 : MinionCardBase<Card_OG_121> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Board().Player(context.card_.GetPlayerIdentifier()).SetNextSpellCostHealthThisTurn();
		}
	};
}

REGISTER_CARD(OG_121)