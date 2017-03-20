#pragma once

namespace Cards
{
	struct Card_CFM_807 : MinionCardBase<Card_CFM_807> {
		static void Battlecry(Contexts::OnPlay context) {
			// TODO: refresh hero power only after playing a spell
			Manipulate(context).HeroPower(context.card_.GetPlayerIdentifier()).SetUsable();
		}
	};
}

REGISTER_CARD(CFM_807)