#pragma once

namespace Cards
{
	struct Card_CFM_940 : SpellCardBase<Card_CFM_940> {
		Card_CFM_940() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				Cards::CardId card_id = DiscoverFromDatabase(context.manipulate_, PreIndexedCards::IndexedType::kMinionTaunt);
				if (!Cards::IsValidCardId(card_id)) return;
				context.manipulate_.CurrentPlayer().AddHandCard(card_id);
			});
		}
	};
}

REGISTER_CARD(CFM_940)
