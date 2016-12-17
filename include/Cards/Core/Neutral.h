#pragma once

#include "Cards/id-map.h"
#include "Cards/MinionCardBase.h"
#include "Cards/MinionCardUtils.h"
#include "Cards/EnchantmentCardBase.h"

namespace Cards
{
	class Card_CS2_189 : public MinionCardBase<Card_CS2_189>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS2_189;

		Card_CS2_189()
		{
			battlecry = [](auto& context) {
				state::CardRef ref = context.GetBattleCryTarget(Targets().Targetable().Enemy(context));
				Damage(context).Target(ref).Amount(1);
			};
		}
	};

	class Card_CS1_042 : public MinionCardBase<Card_CS1_042>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_CS1_042;

		Card_CS1_042()
		{
			Taunt();
		}
	};

	class Card_CS2_168 : public MinionCardBase<Card_CS2_168>
	{
	public:
		static constexpr int id = Cards::ID_CS2_168;

		Card_CS2_168() {}
	};

	class Card_EX1_508_Enchant : public EnchantmentCardBase
	{
	public:
		static constexpr EnchantmentTiers tier = kEnchantmentAura;

		Card_EX1_508_Enchant()
		{
			apply_functor = [](auto& stats) {
				++stats.attack;
			};
		}
	};

	class Card_EX1_508 : public MinionCardBase<Card_EX1_508>, MinionCardUtils
	{
	public:
		static constexpr int id = Cards::ID_EX1_508;

		Card_EX1_508()
		{
			aura_handler.get_targets = [](auto& context) {
				if (!IsAlive(context, context.card_ref_)) return;
				auto targetor = Targets().Owner(context).Minion().Murlocs().Exclude(context.card_ref_);
				targetor.GetInfo().FillTargets(context.state_, context.targets_);
			};
			aura_handler.apply_on = [](auto& context) {
				context.enchant_id_ = Manipulate(context).Card(context.target_).Enchant().Add(Card_EX1_508_Enchant());
			};
			aura_handler.remove_from = [](auto& context) {
				Manipulate(context).Card(context.target_).Enchant().Remove<Card_EX1_508_Enchant>(context.enchant_id_);
			};
		}
	};
}

REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_189)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS1_042)
REGISTER_MINION_CARD_CLASS(Cards::Card_CS2_168)
REGISTER_MINION_CARD_CLASS(Cards::Card_EX1_508)
