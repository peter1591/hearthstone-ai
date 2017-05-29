#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=256&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_EX1_245 : SpellCardBase<Card_EX1_245> {
		Card_EX1_245() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Silence();
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 1);
			});
		}
	};

	struct Card_EX1_251 : SpellCardBase<Card_EX1_251, Overload<2>> {
		Card_EX1_251() {
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

	struct Card_EX1_238 : SpellCardBase<Card_EX1_238, Overload<1>> {
		Card_EX1_238() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 1);
			});
		}
	};

	struct Card_EX1_243 : MinionCardBase<Card_EX1_243, Overload<2>, Windfury> {};

	struct Card_EX1_247 : WeaponCardBase<Card_EX1_247, Overload<1>> {};

	struct Card_CS2_038 : SpellCardBase<Card_CS2_038> {
		Card_CS2_038() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).AddDeathrattle(
					[](FlowControl::deathrattle::context::Deathrattle context) {
					SummonAt(context, context.player_, context.zone_pos_, 
						context.manipulate_.GetCard(context.card_ref_).GetCardId());
				});
			});
		}
	};

	struct Card_CS2_053e : Enchantment<Card_CS2_053e, Cost<-3>> {};
	struct Card_CS2_053 : SpellCardBase<Card_CS2_053> {
		Card_CS2_053() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef card_ref = context.manipulate_.Hero(context.player_).DrawCard();
				if (!card_ref.IsValid()) return;
				context.manipulate_.Card(card_ref).Enchant().Add<Card_CS2_053e>();
			});
		}
	};

	struct Card_EX1_tk11 : MinionCardBase<Card_EX1_tk11, Taunt> {};
	struct Card_EX1_248 : SpellCardBase<Card_EX1_248, Overload<2>> {
		Card_EX1_248() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_tk11);
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_tk11);
			});
		}
	};

	struct Card_EX1_241 : SpellCardBase<Card_EX1_241, Overload<2>> {
		Card_EX1_241() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 5);
			});
		}
	};

	struct Card_EX1_259 : SpellCardBase<Card_EX1_259, Overload<2>> {
		Card_EX1_259() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach(
					[&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_,
						context.manipulate_.GetRandom().Get(2, 3));
				});
			});
		}
	};

	struct Card_EX1_575 : MinionCardBase<Card_EX1_575> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			context.manipulate_.Hero(self).DrawCard();
			return true;
		}
		Card_EX1_575() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_EX1_258e : Enchantment<Card_EX1_258e, Attack<1>, MaxHP<1>> {};
	struct Card_EX1_258 : MinionCardBase<Card_EX1_258> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnPlay::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;
			if (context.manipulate_.GetCard(context.card_ref_).GetRawData().overload > 0) {
				context.manipulate_.OnBoardMinion(self).Enchant().Add<Card_EX1_258e>();
			}
			return true;
		}
		Card_EX1_258() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnPlay>();
		}
	};

	struct Card_EX1_567 : WeaponCardBase<Card_EX1_567, Windfury, Overload<2>> {};

	struct Card_EX1_250 : MinionCardBase<Card_EX1_250, Overload<3>, Taunt> {};

	struct Card_NEW1_010 : MinionCardBase<Card_NEW1_010, Charge, Shield, Taunt, Windfury> {};
}

REGISTER_CARD(NEW1_010)
REGISTER_CARD(EX1_250)
REGISTER_CARD(EX1_567)
REGISTER_CARD(EX1_258)
REGISTER_CARD(EX1_575)
REGISTER_CARD(EX1_259)
REGISTER_CARD(EX1_241)
REGISTER_CARD(EX1_248)
REGISTER_CARD(EX1_tk11)
REGISTER_CARD(CS2_053)
REGISTER_CARD(CS2_038)
REGISTER_CARD(EX1_247)
REGISTER_CARD(EX1_243)
REGISTER_CARD(EX1_238)
REGISTER_CARD(EX1_251)
REGISTER_CARD(EX1_245)
