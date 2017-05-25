#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=128&sort=-cost&display=1
// Last finished card: Defias Bandit

namespace Cards
{
	struct Card_EX1_145 : SpellCardBase<Card_EX1_145> {
		Card_EX1_145() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int turn = context.manipulate_.Board().GetTurn();
				context.manipulate_.AddEvent<state::Events::EventTypes::GetPlayCardCost>(
					[turn](state::Events::EventTypes::GetPlayCardCost::Context context) mutable
				{
					if (context.manipulate_.Board().GetTurn() != turn) return false;

					if (context.manipulate_.GetCard(context.card_ref_).GetCardType() != state::kCardTypeSpell) return true;
					*context.cost_ -= 3;
					return false;
				});
			});
		}
	};

	struct Card_EX1_144e : Enchantment<Card_EX1_144e, Cost<-2>> {};
	struct Card_EX1_144 : SpellCardBase<Card_EX1_144> {
		Card_EX1_144() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Ally().Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				context.manipulate_.OnBoardMinion(target).MoveTo<state::kCardZoneHand>();
				if (context.manipulate_.GetCard(target).GetZone() != state::kCardZoneHand) return;
				context.manipulate_.Card(target).Enchant().Add<Card_EX1_144e>();
			});
		}
	};

	struct Card_CS2_073e : Enchantment<Card_CS2_073e, Attack<2>> {};
	struct Card_CS2_073e2 : Enchantment<Card_CS2_073e2, Attack<4>> {};
	struct Card_CS2_073 : SpellCardBase<Card_CS2_073> {
		Card_CS2_073() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
				if (combo) {
					context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_073e2>();
				}
				else {
					context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_073e>();
				}
			});
		}
	};

	struct Card_EX1_131 : MinionCardBase<Card_EX1_131> {
		static void Battlecry(Contexts::OnPlay const& context) {
			bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
			if (combo) {
				SummonToRight(context.manipulate_, context.card_ref_, Cards::ID_EX1_131t);
			}
		}
	};
}

REGISTER_CARD(EX1_131)
REGISTER_CARD(CS2_073)
REGISTER_CARD(EX1_144)
REGISTER_CARD(EX1_145)