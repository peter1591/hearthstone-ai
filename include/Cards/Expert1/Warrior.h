#pragma once

// http://www.hearthpwn.com/cards?filter-set=3&filter-class=1024&sort=-cost&display=1
// Done

namespace Cards
{
	struct Card_EX1_607e : Enchantment<Card_EX1_607e, Attack<2>> {};
	struct Card_EX1_607 : SpellCardBase<Card_EX1_607> {
		Card_EX1_607() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 1);
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_EX1_607e>();
			});
		}
	};

	struct Card_EX1_410 : SpellCardBase<Card_EX1_410> {
		Card_EX1_410() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef hero_ref = context.manipulate_.Board().Player(context.player_).GetHeroRef();
				int damage = context.manipulate_.GetCard(hero_ref).GetArmor();
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, damage);
			});
		}
	};

	struct Card_EX1_409e : Enchantment<Card_EX1_409e, Attack<1>, MaxHP<1>> {};
	struct Card_EX1_409 : SpellCardBase<Card_EX1_409> {
		Card_EX1_409() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef weapon_ref = context.manipulate_.Board().Player(context.player_).GetWeaponRef();
				if (!weapon_ref.IsValid()) {
					context.manipulate_.Hero(context.player_).EquipWeapon(Cards::ID_EX1_409t);
					return;
				}
				context.manipulate_.Weapon(weapon_ref).Enchant().Add<Card_EX1_409e>();
			});
		}
	};

	struct Card_EX1_392 : SpellCardBase<Card_EX1_392> {
		Card_EX1_392() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				int draw_count = 0;
				
				auto op = [&](state::CardRef card_ref) {
					if (context.manipulate_.GetCard(card_ref).GetDamage() > 0) ++draw_count;
				};

				op(context.manipulate_.Board().Player(context.player_).GetHeroRef());
				context.manipulate_.Board().Player(context.player_).minions_.ForEach(op);

				for (int i = 0; i < draw_count; ++i) {
					context.manipulate_.Hero(context.player_).DrawCard();
				}
			});
		}
	};

	struct Card_NEW1_036 : SpellCardBase<Card_NEW1_036> {
		Card_NEW1_036() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.Hero(context.player_).DrawCard();

				state::PlayerIdentifier player = context.player_;
				int turn = context.manipulate_.Board().GetTurn();
				context.manipulate_.AddEvent<state::Events::EventTypes::OnTakeDamage>(
					[&](state::Events::EventTypes::OnTakeDamage::Context context) {
					if (context.manipulate_.Board().GetTurn() != turn) return false;

					auto const& card = context.manipulate_.GetCard(context.card_ref_);
					if (card.GetCardType() != state::kCardTypeMinion) return true;
					if (card.GetPlayerIdentifier() != player) return true;
					int max_damage = card.GetHP() - 1;
					if (*context.damage_ > max_damage) *context.damage_ = max_damage;
					return true;
				});
			});
		}
	};

	struct Card_CS2_104e : Enchantment<Card_CS2_104e, Attack<3>, MaxHP<3>> {};
	struct Card_CS2_104 : SpellCardBase<Card_CS2_104> {
		Card_CS2_104() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion().Damaged();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Enchant().Add<Card_CS2_104e>();
			});
		}
	};

	struct Card_EX1_391 : SpellCardBase<Card_EX1_391> {
		Card_EX1_391() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_).Minion();
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				context.manipulate_.OnBoardMinion(context.GetTarget()).Damage(context.card_ref_, 2);
				if (context.manipulate_.GetCard(context.GetTarget()).GetHP() <= 0) return;
				context.manipulate_.Hero(context.player_).DrawCard();
			});
		}
	};

	struct Card_EX1_402 : MinionCardBase<Card_EX1_402> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTakeDamage::Context context) {
			if (*context.damage_ <= 0) return true;
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			auto const& card = context.manipulate_.GetCard(self);
			if (card.GetPlayerIdentifier() != player) return true;
			if (card.GetCardType() != state::kCardTypeMinion) return true;

			context.manipulate_.Hero(player).GainArmor(1);
			return true;
		}
		Card_EX1_402() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTakeDamage>();
		}
	};

	struct Card_EX1_603e : Enchantment<Card_EX1_603e, Attack<2>> {};
	struct Card_EX1_603 : MinionCardBase<Card_EX1_603> {
		static bool GetSpecifiedTargets(Contexts::SpecifiedTargetGetter & context) {
			context.SetOptionalBattlecryTargets(context.player_).Minion();
			return true;
		}
		static void Battlecry(Contexts::OnPlay const& context) {
			state::CardRef target = context.GetTarget();
			if (!target.IsValid()) return;
			context.manipulate_.OnBoardMinion(target).Damage(context.card_ref_, 1);
			context.manipulate_.OnBoardMinion(target).Enchant().Add<Card_EX1_603e>();
		}
	};

	struct Card_EX1_604e : Enchantment<Card_EX1_604e, Attack<1>> {};
	struct Card_EX1_604 : MinionCardBase<Card_EX1_604> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::OnTakeDamage::Context context) {
			if (*context.damage_ <= 0) return true;
			auto const& card = context.manipulate_.GetCard(self);
			if (card.GetCardType() != state::kCardTypeMinion) return true;
			context.manipulate_.OnBoardMinion(self).Enchant().Add<Card_EX1_604e>();
			return true;
		}
		Card_EX1_604() {
			RegisterEvent<MinionInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::OnTakeDamage>();
		}
	};

	struct Card_EX1_408 : SpellCardBase<Card_EX1_408> {
		Card_EX1_408() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				context.SetRequiredSpellTargets(context.player_);
				return true;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				state::CardRef hero_ref = context.manipulate_.Board().Player(context.player_).GetHeroRef();

				int damage = 4;
				if (context.manipulate_.GetCard(hero_ref).GetHP() <= 12) damage = 6;
				context.manipulate_.OnBoardCharacter(context.GetTarget()).Damage(context.card_ref_, damage);
			});
		}
	};

	struct Card_EX1_398 : MinionCardBase<Card_EX1_398> {
		static void Battlecry(Contexts::OnPlay const& context) {
			context.manipulate_.Hero(context.player_).EquipWeapon(Cards::ID_EX1_398t);
		}
	};

	struct Card_EX1_407 : SpellCardBase<Card_EX1_407> {
		Card_EX1_407() {
			onplay_handler.SetSpecifyTargetCallback([](Contexts::SpecifiedTargetGetter & context) {
				size_t minions =
					context.manipulate_.Board().FirstPlayer().minions_.Size() +
					context.manipulate_.Board().SecondPlayer().minions_.Size();
				return minions >= 2;
			});
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				size_t first_minions = context.manipulate_.Board().FirstPlayer().minions_.Size();
				size_t second_minions = context.manipulate_.Board().SecondPlayer().minions_.Size();

				size_t minions = first_minions + second_minions;
				if (minions < 2) return;

				size_t minion_idx = context.manipulate_.GetRandom().Get(minions);
				bool first_player = true;

				if (minion_idx >= first_minions) {
					first_player = false;
					minion_idx -= first_minions;
					assert(minion_idx < second_minions);
				}

				state::CardRef winner_ref;
				if (first_player) winner_ref = context.manipulate_.Board().FirstPlayer().minions_.Get(minion_idx);
				else winner_ref = context.manipulate_.Board().SecondPlayer().minions_.Get(minion_idx);

				auto op = [&](state::CardRef card_ref) {
					if (card_ref == winner_ref) return;
					context.manipulate_.OnBoardMinion(card_ref).Destroy();
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(op);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(op);
			});
		}
	};

	struct Card_EX1_411e2 : public Enchantment<Card_EX1_411e2, Attack<-1>> {};
	struct Card_EX1_411e : public EventHookedEnchantment<Card_EX1_411e, Immune> {
		using EventType = state::Events::EventTypes::AfterAttack;
		static void HandleEvent(EventHookedEnchantmentHandler<Card_EX1_411e> const& handler) {
			handler.context.manipulate_.Weapon(handler.card_ref).Enchant().Add<Card_EX1_411e2>();
			handler.RemoveEnchantment();
		}
	};
	struct Card_EX1_411 : WeaponCardBase<Card_EX1_411> {
		static bool HandleEvent(state::CardRef self, state::Events::EventTypes::PrepareAttackTarget::Context context) {
			state::PlayerIdentifier player = context.manipulate_.GetCard(self).GetPlayerIdentifier();
			state::CardRef hero_ref = context.manipulate_.Board().Player(player).GetHeroRef();
			if (context.attacker_ != hero_ref) return true;
			if (context.manipulate_.GetCard(*context.defender_).GetCardType() != state::kCardTypeMinion) return true;

			context.manipulate_.Weapon(self).Enchant().AddEventHooked(Card_EX1_411e());
			return true;
		}
		Card_EX1_411() {
			RegisterEvent<WeaponInPlayZone, NonCategorized_SelfInLambdaCapture,
				state::Events::EventTypes::PrepareAttackTarget>();
		}
	};

	struct Card_EX1_414e : public Enchantment<Card_EX1_414e, Attack<6>> {};
	struct Card_EX1_414 : public MinionCardBase<Card_EX1_414, Charge> {
		static auto GetEnrageTarget(FlowControl::aura::contexts::AuraGetTarget context) {
			context.new_target = context.card_ref_;
		}
		Card_EX1_414() {
			Enrage<Card_EX1_414e>();
		}
	};
}

REGISTER_CARD(EX1_414)
REGISTER_CARD(EX1_411)
REGISTER_CARD(EX1_407)
REGISTER_CARD(EX1_398)
REGISTER_CARD(EX1_408)
REGISTER_CARD(EX1_604)
REGISTER_CARD(EX1_603)
REGISTER_CARD(EX1_402)
REGISTER_CARD(EX1_391)
REGISTER_CARD(CS2_104)
REGISTER_CARD(NEW1_036)
REGISTER_CARD(EX1_392)
REGISTER_CARD(EX1_409)
REGISTER_CARD(EX1_410)
REGISTER_CARD(EX1_607)