#ifndef GAME_ENGINE_CARDS_CARD_CS2_188
#define GAME_ENGINE_CARDS_CARD_CS2_188

DEFINE_CARD_CLASS_START(CS2_188)

// Abusive Sergant

static void GetRequiredTargets(Player const& player, SlotIndexBitmap &targets, bool & meet_requirements)
{
	targets = SlotIndexHelper::GetTargets(player, SlotIndexHelper::TARGET_SPELL_ALL_MINIONS);
	meet_requirements = true; // it's fine even if no target available
}

static void BattleCry(Player & player, Move::PlayMinionData const& play_minion_data)
{
	constexpr int attack_boost = 2;

	if (play_minion_data.target < 0) {
		// no target to buff
		return;
	}

	try {
		auto & buff_target = player.board.object_manager.GetMinion(play_minion_data.target);
		auto enchant = std::make_unique<Enchantment_BuffMinion_C<attack_boost, 0, 0, 0, true>>();
		buff_target.enchantments.Add(std::move(enchant));
	}
	catch (std::out_of_range ex) {
#ifdef DEBUG
		std::cerr << "WARNING: buff target vanished." << std::endl;
#endif
		return;
	}
}

DEFINE_CARD_CLASS_END()
#endif
