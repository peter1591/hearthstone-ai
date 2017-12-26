#pragma once

#include <random>

#include "state/State.h"
#include "engine/view/BoardView.h"

namespace engine
{
	namespace view
	{
		namespace board_view {
			class StateRestorer
			{
			public:
				StateRestorer(BoardView const& board, board_view::UnknownCardsSetsManager & first, UnknownCardsSetsManager & second)
					: board_(board), first_unknown_cards_mgr_(first), second_unknown_cards_mgr_(second)
				{}

				state::State RestoreState(std::mt19937 & rand) {
					first_unknown_cards_mgr_.Prepare(rand);
					second_unknown_cards_mgr_.Prepare(rand);

					state::State state;
					MakePlayer(state::kPlayerFirst, state, rand, board_.GetFirstPlayer(), first_unknown_cards_mgr_);
					MakePlayer(state::kPlayerSecond, state, rand, board_.GetSecondPlayer(), second_unknown_cards_mgr_);
					state.GetMutableCurrentPlayerId().SetFirst(); // AI is helping first player, and should now waiting for an action
					state.SetTurn(board_.GetTurn());

					return state;
				}

			private:
				Cards::CardId GetCardId(std::string const& card_id)
				{
					auto const& container = Cards::Database::GetInstance().GetIdMap();
					auto it = container.find(card_id);
					if (it == container.end()) {
						return Cards::kInvalidCardId;
					}
					return (Cards::CardId)it->second;
				}

				void MakePlayer(state::PlayerIdentifier player, state::State & state, std::mt19937 & random,
					board_view::Player const& board_player, board_view::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
				{
					MakeHero(player, state, board_player.hero);
					MakeHeroPower(player, state, board_player.hero_power);
					MakeDeck(player, state, random, board_player.deck, unknown_cards_sets_mgr);
					MakeHand(player, state, board_player.hand, unknown_cards_sets_mgr);
					MakeMinions(player, state, board_player.minions);
					// TODO: enchantments
					// TODO: weapon
					// TODO: secrets

					state.GetBoard().Get(player).SetFatigueDamage(board_player.fatigue);
					MakeResource(state.GetBoard().Get(player).GetResource(), board_player.resource);
				}

				void ApplyStatus(state::Cards::CardData & raw_card, board_view::CharacterStatus const& status)
				{
					raw_card.enchanted_states.charge = status.charge;
					raw_card.taunt = status.taunt;
					raw_card.shielded = status.divine_shield;
					raw_card.enchanted_states.stealth = status.stealth;
					// TODO: forgetful
					raw_card.enchanted_states.freeze_attack = status.freeze;
					raw_card.freezed = status.frozon;
					raw_card.enchanted_states.poisonous = status.poisonous;
					raw_card.enchanted_states.stealth = status.stealth;
					raw_card.enchanted_states.max_attacks_per_turn = status.max_attacks_per_turn;
				}

				void AddMinion(state::PlayerIdentifier player, state::State & state, board_view::Minion const& minion, int pos)
				{
					state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(minion.card_id);
					raw_card.enchanted_states.player = player;
					raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

					raw_card.enchanted_states.max_hp = minion.max_hp;
					raw_card.damaged = minion.damage;
					raw_card.enchanted_states.attack = minion.attack;
					raw_card.num_attacks_this_turn = minion.attacks_this_turn;
					// TODO: exhausted (needed?)
					raw_card.silenced = minion.silenced;
					raw_card.enchanted_states.spell_damage = minion.spellpower;
					// TODO: enchantments
					ApplyStatus(raw_card, minion.status);

					raw_card.zone = state::kCardZoneNewlyCreated;
					raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states); // TODO: apply a enchantment to make up the states
					state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
					state.GetZoneChanger<state::kCardTypeMinion, state::kCardZoneNewlyCreated>(ref)
						.ChangeTo<state::kCardZonePlay>(player, pos);

					// TODO:
					// Check stats changed after put in board
					// Stats might changed due to some triggers, e.g., just_played flag
					state.GetMutableCard(ref).SetJustPlayedFlag(minion.summoned_this_turn);
					state.GetMutableCard(ref).SetNumAttacksThisTurn(minion.attacks_this_turn);
				}

				void MakeMinions(state::PlayerIdentifier player, state::State & state, board_view::Minions const& minions)
				{
					int pos = 0;
					for (auto const& minion : minions.minions) {
						AddMinion(player, state, minion, pos);
						++pos;
					}
				}

				void MakeResource(state::board::PlayerResource & resource, board_view::Resource const& board_resource)
				{
					resource.SetCurrent(board_resource.current);
					resource.SetTotal(board_resource.total);
					resource.SetCurrentOverloaded(board_resource.overload);
					resource.SetNextOverload(board_resource.overload_next_turn);
				}

				void MakeHero(state::PlayerIdentifier player, state::State & state, board_view::Hero const& board_hero)
				{
					// TODO: mark ctor of state::Cards::CardData as private, to make it only constructible from CardDispatcher::CreateInstance

					state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(board_hero.card_id);

					raw_card.enchanted_states.player = player;
					assert(raw_card.card_type == state::kCardTypeHero);
					raw_card.zone = state::kCardZoneNewlyCreated;
					raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);

					raw_card.enchanted_states.max_hp = board_hero.max_hp;
					raw_card.damaged = board_hero.damage;
					raw_card.armor = board_hero.armor;
					raw_card.enchanted_states.attack = board_hero.attack;
					ApplyStatus(raw_card, board_hero.status);

					// TODO: enchantments

					raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);
					state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
					state.GetZoneChanger<state::kCardTypeHero, state::kCardZoneNewlyCreated>(ref)
						.ChangeTo<state::kCardZonePlay>(player);

					state.GetMutableCard(ref).SetNumAttacksThisTurn(board_hero.attacks_this_turn);
				}

				void MakeHeroPower(state::PlayerIdentifier player, state::State & state, board_view::HeroPower const& hero_power)
				{
					state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(hero_power.card_id);
					assert(raw_card.card_type == state::kCardTypeHeroPower);

					raw_card.zone = state::kCardZoneNewlyCreated;
					state::CardRef ref = state.AddCard(state::Cards::Card(raw_card));
					state.GetZoneChanger<state::kCardTypeHeroPower, state::kCardZoneNewlyCreated>(ref)
						.ChangeTo<state::kCardZonePlay>(player);

					state.GetMutableCard(ref).SetUsable(!hero_power.used);
				}

				void PushBackDeckCard(Cards::CardId id, std::mt19937 & random, state::State & state, state::PlayerIdentifier player)
				{
					int deck_count = (int)state.GetBoard().Get(player).deck_.Size();
					state.GetBoard().Get(player).deck_.ShuffleAdd(id, [&](int exclusive_max) {
						return random() % exclusive_max;
					});
					++deck_count;
					assert(state.GetBoard().Get(player).deck_.Size() == deck_count);
				}

				void MakeDeck(state::PlayerIdentifier player, state::State & state, std::mt19937 & random,
					std::vector<CardInfo> cards,
					board_view::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
				{
					for (auto const& card : cards) {
						Cards::CardId card_id = card.GetCardId(unknown_cards_sets_mgr);
						PushBackDeckCard(card_id, random, state, player);
					}
				}

				state::CardRef AddHandCard(state::PlayerIdentifier player, state::State & state, Cards::CardId card_id)
				{
					state::Cards::CardData raw_card = Cards::CardDispatcher::CreateInstance(card_id);
					raw_card.enchanted_states.player = player;
					raw_card.enchantment_handler.SetOriginalStates(raw_card.enchanted_states);
					raw_card.zone = state::kCardZoneNewlyCreated;

					auto ref = state.AddCard(state::Cards::Card(raw_card));

					int hand_count = (int)state.GetBoard().Get(player).hand_.Size();
					state.GetZoneChanger<state::kCardZoneNewlyCreated>(ref)
						.ChangeTo<state::kCardZoneHand>(player);

					assert(state.GetCardsManager().Get(ref).GetCardId() == card_id);
					assert(state.GetCardsManager().Get(ref).GetPlayerIdentifier() == player);
					if (hand_count == 10) {
						assert(state.GetBoard().Get(player).hand_.Size() == 10);
						assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneGraveyard);
					}
					else {
						++hand_count;
						assert((int)state.GetBoard().Get(player).hand_.Size() == hand_count);
						assert(state.GetCardsManager().Get(ref).GetZone() == state::kCardZoneHand);
						if constexpr (!state::kOrderHandCardsByCardId) {
							assert(state.GetBoard().Get(player).hand_.Get(hand_count - 1) == ref);
							assert(state.GetCardsManager().Get(ref).GetZonePosition() == (hand_count - 1));
						}
					}

					return ref;
				}

				void MakeHand(state::PlayerIdentifier player, state::State & state,
					std::vector<CardInfo> const& cards,
					board_view::UnknownCardsSetsManager const& unknown_cards_sets_mgr)
				{
					for (auto const& card : cards) {
						Cards::CardId card_id = card.GetCardId(unknown_cards_sets_mgr);
						AddHandCard(player, state, card_id);
					}
				}

			private:
				BoardView const& board_;
				board_view::UnknownCardsSetsManager & first_unknown_cards_mgr_;
				board_view::UnknownCardsSetsManager & second_unknown_cards_mgr_;
			};
		}
	}
}