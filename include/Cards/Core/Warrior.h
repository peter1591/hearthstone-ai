#pragma once

namespace Cards
{
	struct Card_CS2_102 : public HeroPowerCardBase<Card_CS2_102> {
		Card_CS2_102() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.Hero(context.manipulate_.Board().GetCard(context.card_ref_).GetPlayerIdentifier())
					.GainArmor(2);
			});
		}
	};

	struct Card_CS2_106 : public WeaponCardBase<Card_CS2_106> {};
}

REGISTER_CARD(CS2_102)
REGISTER_CARD(CS2_106)
