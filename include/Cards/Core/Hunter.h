#pragma once

// http://www.hearthpwn.com/cards?filter-premium=1&filter-set=2&filter-class=8&sort=-cost&display=1
// Last-finished card: Hunter's Mark

namespace Cards
{
	struct Card_DS1_185 : public SpellCardBase<Card_DS1_185> {
		Card_DS1_185() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 2 + spell_damage);
			});
		}
	};

	struct Card_CS2_084e : public Enchantment<Card_CS2_084e, SetMaxHP<1>> {};
	struct Card_CS2_084 : public SpellCardBase<Card_CS2_084> {
		Card_CS2_084() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target = false;
				return TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_084e>();
			});
		}
	};
}

REGISTER_CARD(CS2_084)
REGISTER_CARD(DS1_185)