#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=512&sort=-cost&display=1
// Done.

namespace Cards
{
	struct Card_CS2_056 : public HeroPowerCardBase<Card_CS2_056> {
		Card_CS2_056() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).Damage(context.card_ref_, 2);
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_NEW1_003 : public SpellCardBase<Card_NEW1_003> {
		Card_NEW1_003() {
			onplay_handler.SetSpecifyTargetCallback([](FlowControl::onplay::context::GetSpecifiedTarget const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).SpellTargetableDemons().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_
					.OnBoardCharacter(context.GetTarget())
					.Destroy();
			});
		}
	};

	struct Card_CS2_063e : public EventHookedEnchantment<Card_CS2_063e> {
		static void RegisterEvent(FlowControl::Manipulate & manipulate, state::CardRef card_ref,
			FlowControl::enchantment::Enchantments::IdentifierType id,
			FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData & aux_data)
		{
			manipulate.AddEvent<state::Events::EventTypes::OnTurnStart>([card_ref, id, aux_data](state::Events::EventTypes::OnTurnStart::Context context) {
				if (context.manipulate_.GetCard(card_ref).GetZone() != state::kCardZonePlay) return false;
				if (!context.manipulate_.GetCard(card_ref).GetEnchantmentHandler().Exists(
					FlowControl::enchantment::TieredEnchantments::IdentifierType{ Card_CS2_063e::tier, id })) return false;

				if (context.manipulate_.Board().GetCurrentPlayerId() != aux_data.player) return true;
				context.manipulate_.OnBoardMinion(card_ref).Destroy();
				return true;
			});
		}
	};
	struct Card_CS2_063 : public SpellCardBase<Card_CS2_063> {
		Card_CS2_063() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion().SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				FlowControl::enchantment::Enchantments::EventHookedEnchantment::AuxData aux_data;
				aux_data.player = context.player_;
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().AddEventHooked(
					Card_CS2_063e(), aux_data);
			});
		}
	};

	struct Card_EX1_302 : public SpellCardBase<Card_EX1_302> {
		Card_EX1_302() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion()
					.ExcludeImmune()
					.SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 1 + spell_damage);

				if (context.manipulate_.OnBoardMinion(context.GetTarget()).Alive()) return;
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_308 : public SpellCardBase<Card_EX1_308> {
		Card_EX1_308() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_)
					.ExcludeImmune()
					.SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 4 + spell_damage);
				DiscardOneRandomHandCard(context.manipulate_, context.player_);
			});
		}
	};

	struct Card_CS2_065 : public MinionCardBase<Card_CS2_065, Taunt> {};

	struct Card_EX1_306 : public MinionCardBase<Card_EX1_306> {
		static void Battlecry(Contexts::OnPlay context) {
			DiscardOneRandomHandCard(context.manipulate_, context.player_);
		}
	};

	struct Card_CS2_061 : public SpellCardBase<Card_CS2_061> {
		Card_CS2_061() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_)
					.ExcludeImmune()
					.SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 2 + spell_damage);
				context.manipulate_.Hero(context.player_).Heal(context.card_ref_, 2);
			});
		}
	};

	struct Card_CS2_057 : public SpellCardBase<Card_CS2_057> {
		Card_CS2_057() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter const& context) {
				*context.allow_no_target_ = false;
				*context.targets_ = TargetsGenerator(context.player_).Minion()
					.ExcludeImmune()
					.SpellTargetable().GetInfo();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, 4 + spell_damage);
			});
		}
	};

	struct Card_CS2_062 : public SpellCardBase<Card_CS2_062> {
		Card_CS2_062() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int spell_damage = context.manipulate_.Board().GetSpellDamage(context.player_);
				auto op = [&](state::CardRef card_ref) {
					context.manipulate_.OnBoardCharacter(card_ref).Damage(context.card_ref_, 3 + spell_damage);
				};
				op(context.manipulate_.Board().FirstPlayer().GetHeroRef());
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				op(context.manipulate_.Board().SecondPlayer().GetHeroRef());
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_CS2_064 : public MinionCardBase<Card_CS2_064> {
		static void Battlecry(Contexts::OnPlay context) {
			auto op = [&](state::CardRef card_ref) {
				context.manipulate_.OnBoardCharacter(card_ref).Damage(context.card_ref_, 1);
			};
			op(context.manipulate_.Board().FirstPlayer().GetHeroRef());
			context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
			op(context.manipulate_.Board().SecondPlayer().GetHeroRef());
			context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
		}
	};
}

REGISTER_CARD(CS2_064)
REGISTER_CARD(CS2_062)
REGISTER_CARD(CS2_057)
REGISTER_CARD(CS2_061)
REGISTER_CARD(EX1_306)
REGISTER_CARD(CS2_065)
REGISTER_CARD(EX1_308)
REGISTER_CARD(EX1_302)
REGISTER_CARD(CS2_063)
REGISTER_CARD(NEW1_003)
REGISTER_CARD(CS2_056)