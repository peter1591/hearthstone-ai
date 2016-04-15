#pragma once

DEFINE_CARD_CLASS_START(CS2_013)

// Wild Growth

static void Spell_Go(Player &player, SlotIndex target)
{
	if (player.stat.crystal.GetTotal() < 10) {
		player.stat.crystal.GainEmptyCrystals(1);
	}
	else {
		if (player.hand.GetCount() < 10) {
			player.hand.AddDeterminedCard(CardDatabase::GetInstance().GetCard(CARD_ID_CS2_013t));
		}
		else {
			// should not happen?
		}
	}
}

DEFINE_CARD_CLASS_END()

DEFINE_CARD_CLASS_START(CS2_013t)

static void Spell_Go(Player &player, SlotIndex target)
{
	player.hand.DrawOneCardToHand();
}

DEFINE_CARD_CLASS_END()