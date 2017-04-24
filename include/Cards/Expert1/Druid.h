#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=4&sort=-cost&display=1
// Last finished card: Force of Nature

namespace Cards
{
	struct Card_EX1_161 : public SpellCardBase<Card_EX1_161> {
		Card_EX1_161() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Destroy();
				context.manipulate_.Hero(context.player_.Opposite()).DrawCard();
				context.manipulate_.Hero(context.player_.Opposite()).DrawCard();
			});
		}
	};

	struct Card_EX1_578 : public SpellCardBase<Card_EX1_578> {
		Card_EX1_578() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto & hero = context.manipulate_.Board().Player(context.player_);
				int attack = context.manipulate_.GetCard(hero.GetHeroRef()).GetAttack();
				state::CardRef weapon_ref = hero.GetWeaponRef();
				if (weapon_ref.IsValid()) {
					attack += context.manipulate_.GetCard(weapon_ref).GetAttack();
				}
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, attack);
			});
		}
	};

	struct Card_EX1_160be : public Enchantment<Card_EX1_160be, Attack<1>, MaxHP<1>> {};
	struct Card_EX1_160 : public SpellCardBase<Card_EX1_160> {
		Card_EX1_160() {
			static std::vector<Cards::CardId> choices{
				Cards::ID_EX1_160a,
				Cards::ID_EX1_160b
			};
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				size_t choice = context.manipulate_.GetChooseOneUserAction(choices);
				if (choice == 0) {
					SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_160t);
				}
				else {
					context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
						context.manipulate_.OnBoardMinion(card_ref).Enchant().Add<Card_EX1_160be>();
					});
				}
			});
		}
	};

	struct Card_EX1_154 : public SpellCardBase<Card_EX1_154> {
		Card_EX1_154() {
			static std::vector<Cards::CardId> choices{
				Cards::ID_EX1_154a,
				Cards::ID_EX1_154b
			};
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				size_t choice = context.manipulate_.GetChooseOneUserAction(choices);
				if (choice == 0) {
					context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 3 + spell_damage);
				}
				else {
					context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 1 + spell_damage);
					context.manipulate_.Hero(context.player_).DrawCard();
				}
			});
		}
	};

	struct Card_EX1_155ae : public Enchantment<Card_EX1_155ae, Attack<4>> {};
	struct Card_EX1_155be : public Enchantment<Card_EX1_155be, MaxHP<4>> {};
	struct Card_EX1_155 : public SpellCardBase<Card_EX1_155> {
		Card_EX1_155() {
			static std::vector<Cards::CardId> choices{
				Cards::ID_EX1_155a,
				Cards::ID_EX1_155b
			};
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				size_t choice = context.manipulate_.GetChooseOneUserAction(choices);
				if (choice == 0) {
					context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_EX1_155ae>();
				}
				else {
					context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_EX1_155be>();
					context.manipulate_.OnBoardMinion(context.GetTarget()).Taunt(true);
				}
			});
		}
	};

	struct Card_EX1_570e : public EnchantmentForThisTurn<Card_EX1_570e, Attack<4>> {};
	struct Card_EX1_570 : public SpellCardBase<Card_EX1_570> {
		Card_EX1_570() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Enchant().Add<Card_EX1_570e>();
				context.manipulate_.Hero(context.player_).GainArmor(4);
			});
		}
	};

	struct Card_EX1_158 : public SpellCardBase<Card_EX1_158> {
		Card_EX1_158() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Board().Player(context.player_).minions_.ForEach([&](state::CardRef card_ref) {
					context.manipulate_.OnBoardMinion(card_ref).AddDeathrattle([](FlowControl::deathrattle::context::Deathrattle context) {
						SummonAt(context, context.player_, context.zone_pos_, Cards::ID_EX1_158t);
					});
				});
			});
		}
	};

	struct Card_EX1_166 : public MinionCardBase<Card_EX1_166> {
		static auto GetSpecifiedTargets(Contexts::SpecifiedTargetGetter context) {
			*context.allow_no_target_ = true;
			return TargetsGenerator(context.player_).Minion().Targetable();
		}
		static void Battlecry(Contexts::OnPlay context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;

			static std::vector<Cards::CardId> choices{
				Cards::ID_EX1_166a,
				Cards::ID_EX1_166b
			};
			size_t choice = context.manipulate_.GetChooseOneUserAction(choices);
			if (choice == 0) {
				context.manipulate_.OnBoardMinion(target).Damage(context.card_ref_, 2);
			}
			else {
				context.manipulate_.OnBoardMinion(target).Silence();
			}
		}
	};

	struct Card_EX1_571 : public SpellCardBase<Card_EX1_571> {
		Card_EX1_571() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_158t);
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_158t);
				SummonToRightmost(context.manipulate_, context.player_, Cards::ID_EX1_158t);
			});
		}
	};
}

REGISTER_CARD(EX1_571)
REGISTER_CARD(EX1_166)
REGISTER_CARD(EX1_158)
REGISTER_CARD(EX1_570)
REGISTER_CARD(EX1_155)
REGISTER_CARD(EX1_154)
REGISTER_CARD(EX1_160)
REGISTER_CARD(EX1_578)
REGISTER_CARD(EX1_161)