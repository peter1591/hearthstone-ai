#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=128&sort=-cost&display=1
// Last finished card: (none)

namespace Cards
{
	struct Card_EX1_145 : SpellCardBase<Card_EX1_145> {
		Card_EX1_145() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int turn = context.manipulate_.Board().GetTurn();
				context.manipulate_.AddEvent<state::Events::EventTypes::GetPlayCardCost>(
					[turn](state::Events::EventTypes::GetPlayCardCost::Context context) mutable
				{
					if (context.manipulate_.Board().GetTurn() != turn) return false;

					if (!context.manipulate_.GetCard(context.card_ref_).IsSpellCard()) return true;
					*context.cost_ -= 3;
					return false;
				});
			});
		}
	};
}

REGISTER_CARD(EX1_145)