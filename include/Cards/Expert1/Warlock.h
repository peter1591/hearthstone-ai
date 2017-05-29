#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=512&sort=-cost&display=1
// Last Finished Card: Felguard

namespace Cards
{
	struct Card_CS2_059o : Enchantment<Card_CS2_059o, MaxHP<1>> {};
	struct Card_CS2_059 : MinionCardBase<Card_CS2_059, Stealth> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			if (owner != context.manipulate_.Board().GetCurrentPlayerId()) return true;

			int pos = context.manipulate_.GetCard(self).GetZonePosition();
			int count = (int)context.manipulate_.Board().Player(owner).minions_.Size();

			assert(count > 0);
			
			if (count == 1) return true;

			int pick_pos = context.manipulate_.GetRandom().Get(count - 1);
			if (pick_pos >= pos) ++pick_pos;

			state::CardRef pick_ref = context.manipulate_.Board().Player(owner).minions_.Get(pick_pos);
			context.manipulate_.OnBoardMinion(pick_ref).Enchant().Add<Card_CS2_059o>();
			return true;
		}
		Card_CS2_059() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTurnEnd>();
		}
	};

	struct Card_EX1_319 : MinionCardBase<Card_EX1_319> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Hero(context.player_).Damage(context.card_ref_, 3);
		}
	};

	struct Card_EX1_596e : Enchantment<Card_EX1_596e, Attack<2>, MaxHP<2>> {};
	struct Card_EX1_596 : SpellCardBase<Card_EX1_596> {
		Card_EX1_596() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				bool friendly_demon = false;
				if (context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier() == context.player_) {
					if (context.manipulate_.GetCard(context.card_ref_).GetRace() == state::kCardRaceDemon) {
						friendly_demon = true;
					}
				}
				if (friendly_demon) {
					context.manipulate_.OnBoardMinion(context.card_ref_).Enchant().Add<Card_EX1_596e>();
				}
				else {
					context.manipulate_.OnBoardMinion(context.card_ref_).Damage(context.card_ref_, 2);
				}
			});
		}
	};

	struct Card_EX1_317 : SpellCardBase<Card_EX1_317> {
		Card_EX1_317() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				std::vector<Cards::CardId> possibles;
				context.manipulate_.Board().Player(context.player_).deck_.ForEach([&](int card_id) {
					if (Cards::CardDispatcher::CreateInstance(card_id).card_race == state::kCardRaceDemon) {
						possibles.push_back((Cards::CardId)card_id);
					}
					return true;
				});

				auto draw_from_deck = [&](int card_id) {
					context.manipulate_.Board().Player(context.player_).deck_.SwapCardIdToLast(card_id);
					context.manipulate_.Hero(context.player_).DrawCard();
				};

				if (possibles.size() <= 2) {
					int rest = 2 - (int)possibles.size();
					for (Cards::CardId card_id : possibles) {
						draw_from_deck(card_id);
					}
					for (int i = 0; i < rest; ++i) {
						context.manipulate_.Hero(context.player_).AddHandCard((int)Cards::ID_EX1_317t);
					}
					return;
				}

				auto rand_two = GetRandomTwoNumbers(context.manipulate_, (int)possibles.size());
				draw_from_deck(possibles[rand_two.first]);
				draw_from_deck(possibles[rand_two.second]);
				return;
			});
		}
	};

	struct Card_EX1_301 : MinionCardBase<Card_EX1_301, Taunt> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Board().Player(context.player_).GetResource().DestroyOneCrystal();
		}
	};

	struct Card_EX1_312 : SpellCardBase<Card_EX1_312> {
		Card_EX1_312() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto functor = [&](state::CardRef ref) {
					context.manipulate_.OnBoardMinion(ref).Destroy();
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(functor);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(functor);
			});
		}
	};
}

REGISTER_CARD(EX1_312)

REGISTER_CARD(EX1_301)
REGISTER_CARD(EX1_317)
REGISTER_CARD(EX1_596)
REGISTER_CARD(EX1_319)
REGISTER_CARD(CS2_059)
