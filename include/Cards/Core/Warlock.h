#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=512&sort=-cost&display=1

namespace Cards
{
	struct Card_CS2_056 : public HeroPowerCardBase<Card_CS2_056> {
		Card_CS2_056() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Damage(context.card_ref_, 2);
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};
}

REGISTER_CARD(CS2_056)