#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=1024&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_CS2_102 : public HeroPowerCardBase<Card_CS2_102> {
		Card_CS2_102() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.Hero(context.manipulate_.Board().GetCard(context.card_ref_).GetPlayerIdentifier())
					.GainArmor(2);
			});
		}
	};

	struct Card_CS2_103e2 : public EnchantmentForThisTurn<Card_CS2_103e2, CantAttackHero> {};
	struct Card_CS2_103e : public Enchantment<Card_CS2_103e, Charge> {};
	struct Card_CS2_103 : public SpellCardBase<Card_CS2_103> {
		Card_CS2_103() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				*context.allow_no_target_ = false;
				context.SetTargets(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_CS2_103e>();
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_CS2_103e2>();
			});
		}
	};

	struct Card_EX1_400 : public SpellCardBase<Card_EX1_400> {
		Card_EX1_400() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto op = [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardCharacter(card_ref).Damage(context.card_ref_, 1);
				};
				op(context.manipulate_.Board().FirstPlayer().GetHeroRef());
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				op(context.manipulate_.Board().SecondPlayer().GetHeroRef());
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_CS2_114 : public SpellCardBase<Card_CS2_114> {
		Card_CS2_114() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				if (context.manipulate_.Board().Player(context.player_.Opposite()).minions_.Size() < 2) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto & minions = context.manipulate_.Board().Player(context.player_.Opposite()).minions_;

				auto op = [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, 2);
				};

				if (minions.Size() == 0) return;

				if (minions.Size() == 1) {
					op(minions.Get(0));
					return;
				}

				std::pair<int, int> targets = GetRandomTwoNumbers(context.manipulate_, (int)minions.Size());
				op(minions.Get(targets.first));
				op(minions.Get(targets.second));
			});
		}
	};

	struct Card_CS2_108 : public SpellCardBase<Card_CS2_108> {
		Card_CS2_108() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				*context.allow_no_target_ = false;
				context.SetTargets(context.player_).Minion().Damaged().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Destroy();
			});
		}
	};

	struct Card_CS2_105e : public EnchantmentForThisTurn<Card_CS2_105e, Attack<4>> {};
	struct Card_CS2_105 : public SpellCardBase<Card_CS2_105> {
		Card_CS2_105() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Enchant().Add<Card_CS2_105e>();
			});
		}
	};

	struct Card_EX1_606 : public SpellCardBase<Card_EX1_606> {
		Card_EX1_606() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).GainArmor(5);
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_084e : public Enchantment<Card_EX1_084e, Attack<1>> {};
	struct Card_EX1_084 : public MinionCardBase<Card_EX1_084> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally().Minion().HasCharge() // friendly charge minions
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_EX1_084() {
			Aura<Card_EX1_084e, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};

	struct Card_NEW1_011 : public MinionCardBase<Card_NEW1_011, Charge> {};
}

REGISTER_CARD(NEW1_011)
REGISTER_CARD(EX1_084)
REGISTER_CARD(EX1_606)
REGISTER_CARD(CS2_105)
REGISTER_CARD(CS2_108)
REGISTER_CARD(CS2_114)
REGISTER_CARD(EX1_400)
REGISTER_CARD(CS2_103)
REGISTER_CARD(CS2_102)
