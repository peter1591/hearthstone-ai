#pragma once

namespace Cards
{
	struct Card_OG_026 : MinionCardBase<Card_OG_026> {
		static void Battlecry(Contexts::OnPlay context) {
			context.manipulate_.Board().Player(context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier())
				.GetResource().UnlockOverload();
		}
	};
}

REGISTER_CARD(OG_026)