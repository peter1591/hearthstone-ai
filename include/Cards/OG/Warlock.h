#pragma once

namespace Cards
{
	struct Card_OG_121 : MinionCardBase<Card_OG_121> {
		static void Battlecry(Contexts::OnPlay const& context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			int turn = context.manipulate_.Board().GetTurn();
			context.manipulate_.AddEvent<state::Events::EventTypes::GetPlayCardCost>(
				[turn, player](state::Events::EventTypes::GetPlayCardCost::Context context)
			{
				int turn_now = context.manipulate_.Board().GetTurn();
				if (turn_now > turn) return false;
				assert(turn_now == turn);
				assert(context.manipulate_.Board().GetCurrentPlayerId() == player);

				if (context.manipulate_.GetCard(context.card_ref_).GetCardType() != state::kCardTypeSpell) return true;
				*context.cost_health_instead_ = true;
				return false; // one-time effect
			});
		}
	};
}

REGISTER_CARD(OG_121)