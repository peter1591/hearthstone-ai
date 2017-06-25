#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=512&sort=-cost&display=1
// Done

namespace Cards
{
	struct Card_CS2_059o : Enchantment<Card_CS2_059o, MaxHP<1>> {};
	struct Card_CS2_059 : MinionCardBase<Card_CS2_059, Stealth> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTurnEnd::Context const& context) {
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
				context.SetRequiredSpellTargets(context.player_).Minion();
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
				context.manipulate_.Board().Player(context.player_).deck_.ForEach([&](Cards::CardId in_card_id) {
					if (Cards::CardDispatcher::CreateInstance(in_card_id).card_race == state::kCardRaceDemon) {
						possibles.push_back(in_card_id);
					}
					return true;
				});

				auto draw_from_deck = [&](Cards::CardId in_card_id) {
					context.manipulate_.Board().Player(context.player_).deck_.SwapCardIdToLast(in_card_id);
					context.manipulate_.Player(context.player_).DrawCard();
				};

				if (possibles.size() <= 2) {
					int rest = 2 - (int)possibles.size();
					for (Cards::CardId card_id : possibles) {
						draw_from_deck(card_id);
					}
					for (int i = 0; i < rest; ++i) {
						context.manipulate_.Player(context.player_).AddHandCard(Cards::ID_EX1_317t);
					}
					return;
				}

				auto rand_two = GetRandomTwoNumbers(context.manipulate_, (int)possibles.size());
				draw_from_deck(possibles[rand_two.first]);
				draw_from_deck(possibles[rand_two.second]);
			});
		}
	};

	struct Card_EX1_301 : MinionCardBase<Card_EX1_301, Taunt> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Board().Player(context.player_).GetResource().DestroyOneCrystal();
		}
	};

	template <int v> struct Card_EX1_304e1 : Enchantment<Card_EX1_304e1<v>, Attack<v>> {};
	template <int v> struct Card_EX1_304e2 : Enchantment<Card_EX1_304e2<v>, MaxHP<v>> {};
	struct Card_EX1_304 : MinionCardBase<Card_EX1_304> {
		static void Battlecry(Contexts::OnPlay const& context) {
			int attack = 0;
			int hp = 0;
			ApplyToAdjacent(context.manipulate_, context.card_ref_, [&](state::CardRef card_ref) {
				attack += context.manipulate_.GetCard(card_ref).GetAttack();
				hp += context.manipulate_.GetCard(card_ref).GetHP();
			});
			ApplyToAdjacent(context.manipulate_, context.card_ref_, [&](state::CardRef card_ref) {
				context.manipulate_.OnBoardMinion(card_ref).Destroy();
			});
			context.manipulate_.OnBoardMinion(context.card_ref_).Enchant().Add<Card_EX1_304e1>(attack);
			context.manipulate_.OnBoardMinion(context.card_ref_).Enchant().Add<Card_EX1_304e2>(hp);
		}
	};

	struct Card_EX1_303 : SpellCardBase<Card_EX1_303> {
		Card_EX1_303() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Ally().Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int attack = context.manipulate_.GetCard(context.GetTarget()).GetAttack();
				context.manipulate_.Board().Player(context.player_.Opposite()).minions_.ForEach(
					[&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, attack);
				});
				context.manipulate_.OnBoardMinion(context.GetTarget()).Destroy();
			});
		}
	};

	struct Card_EX1_313 : MinionCardBase<Card_EX1_313> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Hero(context.player_).Damage(context.card_ref_, 5);
		}
	};

	struct Card_EX1_315o : Enchantment<Card_EX1_315o> {
		static constexpr EnchantmentTiers aura_tier = EnchantmentTiers::kEnchantmentTier3; // always apply at the last stage
		Card_EX1_315o() {
			apply_functor = [](FlowControl::enchantment::Enchantments::ApplyFunctorContext const& context) {
				context.stats_->cost -= 2;
				if (context.stats_->cost <= 0) context.stats_->cost = 1;
			};
		}
	};
	struct Card_EX1_315 : MinionCardBase<Card_EX1_315> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::GetPlayCardCost::Context const& context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			auto const& card = context.manipulate_.GetCard(context.card_ref_);
			if (card.GetPlayerIdentifier() != owner) return true;
			*context.cost_ -= 2;
			if (*context.cost_ <= 0) *context.cost_ = 1;
			return true;
		}
		Card_EX1_315() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::GetPlayCardCost>();
		}
	};

	struct Card_EX1_320 : SpellCardBase<Card_EX1_320> {
		Card_EX1_320() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 2);
				if (context.manipulate_.GetCard(context.GetTarget()).GetZone() == state::kCardZonePlay) {
					if (context.manipulate_.GetCard(context.GetTarget()).GetHP() > 0) {
						return;
					}
				}
				Cards::CardId card_id = GetRandomCardFromDatabase(context.manipulate_, Cards::Database::kMinionDemons);
				SummonToRightmost(context.manipulate_, context.player_, card_id);
			});
		}
	};

	struct Card_EX1_310 : MinionCardBase<Card_EX1_310, Charge> {
		static void Battlecry(Contexts::OnPlay const& context) {
			DiscardOneRandomHandCard(context.manipulate_, context.player_);
			DiscardOneRandomHandCard(context.manipulate_, context.player_);
		}
	};

	struct Card_EX1_309 : SpellCardBase<Card_EX1_309> {
		Card_EX1_309() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Destroy();
				context.manipulate_.Hero(context.player_).Heal(context.card_ref_, 3);
			});
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

	struct Card_EX1_tk33 : HeroPowerCardBase<Card_EX1_tk33> {
		Card_EX1_tk33() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				if (context.manipulate_.Board().Player(context.player_).minions_.Full()) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_tk34);
			});
		}
	};

	struct Card_EX1_323 : MinionCardBase<Card_EX1_323> {
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef hero_ref = context.manipulate_.Hero(context.player_).Transform(Cards::ID_EX1_323h);
			*context.new_card_ref = hero_ref;

			context.manipulate_.Player(context.player_).EquipWeapon(Cards::ID_EX1_323w);
			context.manipulate_.Player(context.player_).ReplaceHeroPower(Cards::ID_EX1_tk33);
			context.manipulate_.OnBoardMinion(context.card_ref_).MoveTo<state::kCardZoneSetASide>();
		}
	};
}

REGISTER_CARD(EX1_tk33)
REGISTER_CARD(EX1_323)
REGISTER_CARD(EX1_312)
REGISTER_CARD(EX1_309)
REGISTER_CARD(EX1_310)
REGISTER_CARD(EX1_320)
REGISTER_CARD(EX1_315)
REGISTER_CARD(EX1_313)
REGISTER_CARD(EX1_303)
REGISTER_CARD(EX1_304)
REGISTER_CARD(EX1_301)
REGISTER_CARD(EX1_317)
REGISTER_CARD(EX1_596)
REGISTER_CARD(EX1_319)
REGISTER_CARD(CS2_059)
