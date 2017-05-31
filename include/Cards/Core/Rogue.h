#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=128&sort=-cost&display=1
// DONE

namespace Cards
{
	struct Card_CS2_083b : public HeroPowerCardBase<Card_CS2_083b> {
		Card_CS2_083b() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef weapon_ref = context.manipulate_.Board().AddCardById(Cards::ID_CS2_082, context.player_);
				context.manipulate_.Hero(context.player_).EquipWeapon<state::kCardZoneNewlyCreated>(weapon_ref);
			});
		}
	};

	struct Card_CS2_072 : public SpellCardBase<Card_CS2_072> {
		Card_CS2_072() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetOptionalTargets(context.player_)
					.Minion().SpellTargetable().AttackLessOrEqualTo(3)
					.GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Damage(context.card_ref_, 2);
			});
		}
	};

	struct Card_CS2_074e : public Enchantment<Card_CS2_074e, Attack<2>> {};
	struct Card_CS2_074 : public SpellCardBase<Card_CS2_074> {
		Card_CS2_074() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				if (!context.manipulate_.Board().Player(context.player_).GetWeaponRef().IsValid()) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef weapon_ref = context.manipulate_.Board().Player(context.player_).GetWeaponRef();
				if (!weapon_ref.IsValid()) return;
				context.manipulate_.Weapon(weapon_ref).Enchant().Add<Card_CS2_074e>();
			});
		}
	};

	struct Card_CS2_075 : public SpellCardBase<Card_CS2_075> {
		Card_CS2_075() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 3);
			});
		}
	};

	struct Card_EX1_581 : public SpellCardBase<Card_EX1_581> {
		Card_EX1_581() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_)
					.Minion().SpellTargetable()
					.GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardMinion(target).MoveTo<state::kCardZoneHand>(context.player_.Opposite());
			});
		}
	};

	struct Card_EX1_278 : public SpellCardBase<Card_EX1_278> {
		Card_EX1_278() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_)
					.SpellTargetable()
					.GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 1);
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_129 : public SpellCardBase<Card_EX1_129> {
		Card_EX1_129() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, 1);
				});
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_CS2_076 : public SpellCardBase<Card_CS2_076> {
		Card_CS2_076() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_)
					.Enemy()
					.Minion().SpellTargetable()
					.GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				context.manipulate_.OnBoardMinion(target).Destroy();
			});
		}
	};

	struct Card_NEW1_004 : public SpellCardBase<Card_NEW1_004> {
		Card_NEW1_004() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).MoveTo<state::kCardZoneHand>(context.player_);
				});
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).MoveTo<state::kCardZoneHand>(context.player_.Opposite());
				});
			});
		}
	};

	struct Card_CS2_077 : public SpellCardBase<Card_CS2_077> {
		Card_CS2_077() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).DrawCard();
				context.manipulate_.Hero(context.player_).DrawCard();
				context.manipulate_.Hero(context.player_).DrawCard();
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};
}

REGISTER_CARD(CS2_077)
REGISTER_CARD(NEW1_004)
REGISTER_CARD(CS2_076)
REGISTER_CARD(EX1_129)
REGISTER_CARD(EX1_278)
REGISTER_CARD(EX1_581)
REGISTER_CARD(CS2_075)
REGISTER_CARD(CS2_074)
REGISTER_CARD(CS2_072)
REGISTER_CARD(CS2_083b)
