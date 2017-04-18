#pragma once

namespace Cards
{
	struct Card_EX1_169 : public SpellCardBase<Card_EX1_169> {
		Card_EX1_169() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int old_current = context.manipulate_.Board().Player(context.player_).GetResource().GetCurrent();
				context.manipulate_.Board().Player(context.player_).GetResource().SetCurrent(old_current + 2);
			});
		}
	};

	struct Card_CS2_008 : public SpellCardBase<Card_CS2_008> {
		Card_CS2_008() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 1 + spell_damage);
			});
		}
	};

	struct Card_CS2_005o : public EnchantmentForThisTurn<Card_CS2_005o, Attack<2>> {};
	struct Card_CS2_005 : public SpellCardBase<Card_CS2_005> {
		Card_CS2_005() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef hero_ref = context.manipulate_.Board().Player(context.player_).GetHeroRef();
				context.manipulate_.Hero(hero_ref).Enchant().Add<Card_CS2_005o>();
				context.manipulate_.Hero(hero_ref).GainArmor(2);
			});
		}
	};

	struct Card_CS2_009e : public Enchantment<Card_CS2_009e, Attack<2>, MaxHP<2>> {};
	struct Card_CS2_009 : public SpellCardBase<Card_CS2_009> {
		Card_CS2_009() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Taunt(true);
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_009e>();
			});
		}
	};
}

REGISTER_CARD(CS2_009)
REGISTER_CARD(CS2_005)
REGISTER_CARD(CS2_008)
REGISTER_CARD(EX1_169)