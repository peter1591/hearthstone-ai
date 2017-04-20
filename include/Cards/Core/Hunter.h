#pragma once

// http://www.hearthpwn.com/cards?filter-premium=1&filter-set=2&filter-class=8&sort=-cost&display=1
// DONE

namespace Cards
{
	struct Card_DS1h_292 : public HeroPowerCardBase<Card_DS1h_292> {
		Card_DS1h_292() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_.Opposite()).Damage(context.card_ref_, 2);
			});
		}
	};

	struct Card_DS1_185 : public SpellCardBase<Card_DS1_185> {
		Card_DS1_185() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
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
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;
				context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_CS2_084e>();
			});
		}
	};

	struct Card_DS1_184 : public SpellCardBase<Card_DS1_184> {
		Card_DS1_184() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto & deck = context.manipulate_.Board().Player(context.player_).deck_;
				std::vector<Cards::CardId> cards;
				for (int i = 0; i < 3; ++i) {
					if (deck.Empty()) break;
					cards.push_back((Cards::CardId)deck.GetLast());
					deck.RemoveLast();
				}
				if (cards.empty()) return;
				size_t choice = context.manipulate_.GetChooseOneUserAction(cards);
				context.manipulate_.Hero(context.player_).AddHandCard(cards[choice]);
			});
		}
	};

	struct Card_DS1_175o : public Enchantment<Card_DS1_175o, Attack<1>> {};
	struct Card_DS1_175 : public MinionCardBase<Card_DS1_175> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion().Beasts() // friendly beasts
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_DS1_175() {
			Aura<Card_DS1_175o, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};

	struct Card_NEW1_032 : public MinionCardBase<Card_NEW1_032, Taunt> {};

	struct Card_NEW1_033o : public Enchantment<Card_NEW1_033o, Attack<1>> {};
	struct Card_NEW1_033 : public MinionCardBase<Card_NEW1_033> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion() // friendly minions
				.Exclude(context.card_ref_) // only apply on other
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_NEW1_033() {
			Aura<Card_NEW1_033o, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};

	struct Card_NEW1_034 : public MinionCardBase<Card_NEW1_034, Charge> {};

	struct Card_NEW1_031 : public SpellCardBase<Card_NEW1_031> {
		Card_NEW1_031() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				if (context.manipulate_.Board().Player(context.player_).minions_.Full()) {
					return false;
				}
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				std::array<Cards::CardId, 3> cards{
					Cards::ID_NEW1_032,
					Cards::ID_NEW1_033,
					Cards::ID_NEW1_034
				};
				int rand = context.manipulate_.GetRandom().Get(3);
				Cards::CardId card_id = cards[rand];

				SummonToRightmost(context.manipulate_, context.player_, card_id);
			});
		}
	};

	struct Card_EX1_539 : public SpellCardBase<Card_EX1_539> {
		Card_EX1_539() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef target = context.GetTarget();
				if (!target.IsValid()) return;

				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				int damage = 3;
				
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					if (context.manipulate_.GetCard(card_ref).GetRace() == state::kCardRaceBeast) {
						damage = 5;
						return false;
					}
					return true;
				});

				context.manipulate_.OnBoardCharacter(target).Damage(context.card_ref_, spell_damage + damage);
			});
		}
	};

	struct Card_DS1_183 : public SpellCardBase<Card_DS1_183> {
		Card_DS1_183() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				if (context.manipulate_.Board().Player(context.player_.Opposite()).minions_.Size() < 2) return false;
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				auto & minions = context.manipulate_.Board().Player(context.player_.Opposite()).minions_;

				auto op = [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).Damage(context.card_ref_, spell_damage + 3);
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

	struct Card_DS1_070o : public Enchantment<Card_DS1_070o, Attack<2>, MaxHP<2>> {};
	struct Card_DS1_070 : public MinionCardBase<Card_DS1_070> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			*context.allow_no_target_ = true;
			return TargetsGenerator(context.player_).Ally(context.player_).Minion().Beasts().Exclude(context.card_ref_);
		}
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_DS1_070o>();
		}
	};

	struct Card_CS2_237 : public MinionCardBase<Card_CS2_237> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::AfterMinionSummoned::Context context) {
			state::PlayerIdentifier owner = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::PlayerIdentifier summon_player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			if (owner != summon_player) return true;

			if (context.manipulate_.GetCard(context.card_ref_).GetRace() != state::kCardRaceBeast) return true;
			context.manipulate_.Hero(owner).DrawCard();
			return true;
		}
		Card_CS2_237() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::AfterMinionSummoned>();
		}
	};

	struct Card_DS1_178e : public Enchantment<Card_DS1_178e, Charge> {};
	struct Card_DS1_178 : public MinionCardBase<Card_DS1_178> {
		static auto GetAuraTargets(FlowControl::aura::contexts::AuraGetTargets context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(context.card_ref_).GetPlayerIdentifier();
			TargetsGenerator(player)
				.Ally(context).Minion().Beasts() // friendly beasts
				.GetInfo().Fill(context.manipulate_, context.new_targets);
		}
		Card_DS1_178() {
			Aura<Card_DS1_178e, EmitWhenAlive, FlowControl::aura::kUpdateWhenMinionChanges>();
		}
	};
}

REGISTER_CARD(DS1_178)
REGISTER_CARD(CS2_237)
REGISTER_CARD(DS1_070)
REGISTER_CARD(DS1_183)
REGISTER_CARD(EX1_539)
REGISTER_CARD(NEW1_031)
REGISTER_CARD(NEW1_032)
REGISTER_CARD(NEW1_033)
REGISTER_CARD(NEW1_034)
REGISTER_CARD(DS1_175)
REGISTER_CARD(DS1_184)
REGISTER_CARD(CS2_084)
REGISTER_CARD(DS1_185)
REGISTER_CARD(DS1h_292)
