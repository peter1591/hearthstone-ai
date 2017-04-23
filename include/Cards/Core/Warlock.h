#pragma once

// http://www.hearthpwn.com/cards?filter-set=2&filter-class=512&sort=-cost&display=1
// Last finished card: Corruption

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
}

REGISTER_CARD(CS2_063)
REGISTER_CARD(NEW1_003)
REGISTER_CARD(CS2_056)