#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=64&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_CS1h_001 : public HeroPowerCardBase<Card_CS1h_001> {
		Card_CS1h_001() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardCharacter(context.GetTarget())
					.Heal(context.card_ref_, 2);
			});
		}
	};

	struct Card_CS1h_001_H1 : public Card_CS1h_001 {};

	struct Card_CS1_130 : public SpellCardBase<Card_CS1_130> {
		Card_CS1_130() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 2);
			});
		}
	};

	struct Card_CS2_003 : public SpellCardBase<Card_CS2_003> {
		Card_CS2_003() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				auto& hand_from = context.manipulate_.Board().Player(context.player_.Opposite()).hand_;
				if (hand_from.Empty()) return;

				size_t rand = context.manipulate_.GetRandom().Get(hand_from.Size());
				Cards::CardId card_id = context.manipulate_.GetCard(hand_from.Get(rand)).GetCardId();
				context.manipulate_.Player(context.player_).AddHandCard(card_id);
			});
		}
	};

	struct Card_CS2_004e : public Enchantment<Card_CS2_004e, MaxHP<2>> {};
	struct Card_CS2_004 : public SpellCardBase<Card_CS2_004> {
		Card_CS2_004() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Player(context.player_).DrawCard();
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_004e>();
			});
		}
	};

	struct Card_CS2_235 : public MinionCardBase<Card_CS2_235> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnHeal::Context const& context) {
			assert(context.amount_ > 0);
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			context.manipulate_.Player(owner).DrawCard();
			return true;
		}
		Card_CS2_235() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnHeal>();
		}
	};

	template <int v>
	struct Card_CS2_236e : public Enchantment<Card_CS2_236e<v>, MaxHP<v>> {};
	struct Card_CS2_236 : public SpellCardBase<Card_CS2_236> {
		Card_CS2_236() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				int v = context.manipulate_.GetCard(target).GetHP();
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_236e>(v);
			});
		}
	};

	struct Card_DS1_233 : public SpellCardBase<Card_DS1_233> {
		Card_DS1_233() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 5);
			});

		}
	};

	struct Card_CS2_234 : public SpellCardBase<Card_CS2_234> {
		Card_CS2_234() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_)
					.Enemy().Minion().AttackLessOrEqualTo(3);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Destroy();
			});
		}
	};

	struct Card_EX1_622 : public SpellCardBase<Card_EX1_622> {
		Card_EX1_622() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_)
					.Enemy()
					.Minion().AttackGreaterOrEqualTo(5);
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Destroy();
			});
		}
	};

	struct Card_CS1_112 : public SpellCardBase<Card_CS1_112> {
		Card_CS1_112() {
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Heal(context.card_ref_, 2);
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Heal(context.card_ref_, 2);
					return true;
				});

				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 2 + spell_damage);
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, 2 + spell_damage);
					return true;
				});
			});
		}
	};

	struct Card_CS1_113 : public SpellCardBase<Card_CS1_113> {
		Card_CS1_113() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Enemy().Minion();
			});
			onplay_handler.SetOnPlayCallback([](engine::FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).ChangeOwner(context.player_);
			});
		}
	};
}

REGISTER_CARD(CS1_113)
REGISTER_CARD(CS1_112)
REGISTER_CARD(EX1_622)
REGISTER_CARD(CS2_234)
REGISTER_CARD(DS1_233)
REGISTER_CARD(CS2_236)
REGISTER_CARD(CS2_235)
REGISTER_CARD(CS2_004)
REGISTER_CARD(CS2_003)
REGISTER_CARD(CS1_130)
REGISTER_CARD(CS1h_001)
REGISTER_CARD(CS1h_001_H1)