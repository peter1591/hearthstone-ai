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

	struct Card_CS2_013 : public SpellCardBase<Card_CS2_013> {
		Card_CS2_013() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_).GetResource().GainEmptyCrystal(1);
			});
		}
	};

	struct Card_CS2_007 : public SpellCardBase<Card_CS2_007> {
		Card_CS2_007() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Heal(context.card_ref_, 8);
			});
		}
	};

	struct Card_CS2_011o : public EnchantmentForThisTurn<Card_CS2_011o, Attack<2>> {};
	struct Card_CS2_011 : public SpellCardBase<Card_CS2_011> {
		Card_CS2_011() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Enchant().Add<Card_CS2_011o>();
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Enchant().Add<Card_CS2_011o>();
				});
			});
		}
	};

	struct Card_CS2_012 : public SpellCardBase<Card_CS2_012> {
		Card_CS2_012() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).Enemy(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				state::CardRef target = context.GetTarget();
				state::PlayerIdentifier player = context.manipulate_.GetCard(target).GetPlayerIdentifier();
				if (target.IsValid()) context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, spell_damage + 4);

				auto op = [&](state::CardRef card_ref) {
					if (card_ref == target) return;
					context.manipulate_.OnBoardCharacter(card_ref).Damage(context.card_ref_, spell_damage + 1);
				};
				op(context.manipulate_.Board().Player(player).GetHeroRef());
				context.manipulate_.Board().Player(player).minions_.ForEach(op);
			});
		}
	};
}

REGISTER_CARD(CS2_012)
REGISTER_CARD(CS2_011)
REGISTER_CARD(CS2_007)
REGISTER_CARD(CS2_013)
REGISTER_CARD(CS2_009)
REGISTER_CARD(CS2_005)
REGISTER_CARD(CS2_008)
REGISTER_CARD(EX1_169)