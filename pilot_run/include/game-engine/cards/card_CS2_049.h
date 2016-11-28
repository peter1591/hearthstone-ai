#pragma once

DEFINE_CARD_CLASS_START(CS2_049)

// Shaman Hero Power: Totemic Call

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	if (player.minions.GetMinionCount() > 7) throw std::runtime_error("logic error");
	if (player.minions.GetMinionCount() == 7) meet_requirements = false;
	else meet_requirements = true;
}

static void HeroPower_Go(Player &player, SlotIndex target)
{
	constexpr int summon_ids_size = 4;
	const static int summon_ids[summon_ids_size] = { CARD_ID_CS2_050, CARD_ID_CS2_051, CARD_ID_CS2_052, CARD_ID_NEW1_009 };

	int r = player.board.random_generator.GetRandom(summon_ids_size);
	Card summonning_card = CardDatabase::GetInstance().GetCard(summon_ids[r]);
	StageHelper::SummonMinion(summonning_card, player.minions.GetEndIterator());
}

DEFINE_CARD_CLASS_END()