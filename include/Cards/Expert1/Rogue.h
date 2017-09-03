#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=128&sort=-cost&display=1
// Done

namespace Cards
{
	struct Card_EX1_145 : SpellCardBase<Card_EX1_145> {
		Card_EX1_145() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int turn = context.manipulate_.Board().GetTurn();
				context.manipulate_.AddEvent<state::Events::EventTypes::GetPlayCardCost>(
					[turn](state::Events::EventTypes::GetPlayCardCost::Context const& context) mutable
				{
					if (context.state_.GetTurn() != turn) return false;

					if (context.state_.GetCard(context.card_ref_).GetCardType() != state::kCardTypeSpell) return true;
					*context.cost_ -= 3;
					return false;
				});
			});
		}
	};

	struct Card_EX1_144e : Enchantment<Card_EX1_144e, Cost<-2>> {};
	struct Card_EX1_144 : SpellCardBase<Card_EX1_144> {
		Card_EX1_144() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Ally().Minion();
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
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
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

	struct Card_EX1_126 : SpellCardBase<Card_EX1_126> {
		Card_EX1_126() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Enemy().Minion();
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				assert(target.IsValid());
				if (context.manipulate_.GetCard(target).GetZone() != state::kCardZonePlay) return;
				int attack = context.manipulate_.GetCard(target).GetAttack();
				ApplyToAdjacent(context.manipulate_, target, [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(target, attack);
				});
			});
		}
	};

	struct Card_EX1_124 : SpellCardBase<Card_EX1_124> {
		Card_EX1_124() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(
					context.card_ref_, combo ? 4 : 2);
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

	struct Card_EX1_522 : MinionCardBase<Card_EX1_522, Stealth, Poisonous> {};

	struct Card_EX1_133 : WeaponCardBase<Card_EX1_133> {
		Card_EX1_133() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredBattlecryTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(
					context.card_ref_, combo ? 2 : 1);
			});
		}
	};

	struct Card_EX1_137 : SpellCardBase<Card_EX1_137> {
		Card_EX1_137() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 2);
				bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
				if (!combo) return;

				state::PlayerIdentifier player = context.player_;
				context.manipulate_.AddEvent<state::Events::EventTypes::OnTurnStart>(
					[player](state::Events::EventTypes::OnTurnStart::Context const& context) {
					if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;
					context.manipulate_.Player(player).AddHandCard(Cards::ID_EX1_137);
					return false;
				});
			});
		}
	};

	template <int v> struct Card_EX1_613e : Enchantment<Card_EX1_613e<v>, Attack<v * 2>, MaxHP<v * 2>> {};
	struct Card_EX1_613 : MinionCardBase<Card_EX1_613> {
		static void Battlecry(Contexts::OnPlay const& context) {
			int played_cards = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_;
			context.manipulate_.OnBoardMinion(context.card_ref_)
				.Enchant().Add<Card_EX1_613e>(played_cards);
		}
	};

	struct Card_EX1_134 : MinionCardBase<Card_EX1_134> {
		Card_EX1_134() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				bool combo = context.GetState().GetBoard().Get(context.player_).played_cards_this_turn_ > 0;
				if (!combo) return;
				context.SetRequiredBattlecryTargets(context.player_);
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, 2);
			});
		}
	};

	struct Card_CS2_233 : SpellCardBase<Card_CS2_233> {
		Card_CS2_233() {
			onplay_handler.SetCheckPlayableCallback([](Contexts::CheckPlayable & context) {
				if (!context.state_.GetBoard().Get(context.player_).GetWeaponRef().IsValid()) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef weapon_ref = context.manipulate_.Board().Player(context.player_).GetWeaponRef();
				if (!weapon_ref.IsValid()) return;
				int attack = context.manipulate_.GetCard(weapon_ref).GetAttack();
				context.manipulate_.Player(context.player_).DestroyWeapon();
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach(
					[&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, attack);
				});
			});
		}
	};

	struct Card_NEW1_014 : MinionCardBase<Card_NEW1_014> {
		static void GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_).Ally().Minion();
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardMinion(target).Stealth(true);

			state::CardRef card_ref = context.card_ref_;
			context.manipulate_.AddEvent<state::Events::EventTypes::OnTurnStart>(
				[card_ref](state::Events::EventTypes::OnTurnStart::Context const& context) {
				state::PlayerIdentifier player = context.manipulate_.GetCard(card_ref).GetPlayerIdentifier();
				if (context.manipulate_.Board().GetCurrentPlayerId() != player) return true;
				if (context.manipulate_.GetCard(card_ref).GetZone() != state::kCardZonePlay) return false;
				context.manipulate_.OnBoardMinion(card_ref).Stealth(false);
				return false;
			});
		}
	};

	struct Card_NEW1_005 : MinionCardBase<Card_NEW1_005> {
		static void GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			bool combo = context.GetState().GetBoard().Get(context.player_).played_cards_this_turn_ > 0;
			if (!combo) return;
			context.SetOptionalBattlecryTargets(context.player_).Minion();
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			bool combo = context.manipulate_.Board().Player(context.player_).played_cards_this_turn_ > 0;
			if (!combo) return;
			if (!context.GetTarget().IsValid()) return;
			context.manipulate_.OnBoardMinion(context.GetTarget()).MoveTo<state::kCardZoneHand>();
		}
	};
}

REGISTER_CARD(NEW1_005)
REGISTER_CARD(NEW1_014)
REGISTER_CARD(CS2_233)
REGISTER_CARD(EX1_134)
REGISTER_CARD(EX1_613)
REGISTER_CARD(EX1_137)
REGISTER_CARD(EX1_133)
REGISTER_CARD(EX1_522)
REGISTER_CARD(EX1_131)
REGISTER_CARD(EX1_124)
REGISTER_CARD(EX1_126)
REGISTER_CARD(CS2_073)
REGISTER_CARD(EX1_144)
REGISTER_CARD(EX1_145)