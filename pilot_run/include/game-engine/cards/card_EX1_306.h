#pragma once

DEFINE_CARD_CLASS_START(EX1_306)

// Succubus

static void BattleCry(Player & player, Move::PlayMinionData const& play_minion_data)
{
	// discard a random card
	// TODO: refactor out as a convenient function
	if (player.hand.GetCount() > 0) {
		int hand_card_idx = player.board.random_generator.GetRandom(player.hand.GetCount());
		player.hand.DiscardHandCard(hand_card_idx);
	}
}

DEFINE_CARD_CLASS_END()