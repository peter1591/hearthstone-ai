#include <iostream>
#include <stdexcept>

#include "board.h"

void Board::GetGameFlowMove(std::vector<Move> &moves) const
{
	Move move;
	move.action = Move::ACTION_GAME_FLOW;
	moves.push_back(move);
}

void Board::GetBoardMoves(std::vector<Move> &moves) const
{
	bool can_play_minion = !this->player_minions.IsFull();
	Move move;

	moves.clear();
	moves.reserve(1 + // end turn
		this->player_hand.GetCards().size()); // play a card from hand

	// the choice to end turn
	move.action = Move::ACTION_END_TURN;
	moves.push_back(move);

	// the choices to play a card from hand
	move.action = Move::ACTION_PLAY_HAND_CARD;
	for (size_t hand_idx = 0; hand_idx < this->player_hand.GetCards().size(); ++hand_idx)
	{
		const Card &playing_card = this->player_hand.GetCards()[hand_idx];
		if (playing_card.type == Card::MINION && !can_play_minion) {
			continue;
		}

		move.data.play_hand_card_data.idx = (int)hand_idx;
		moves.push_back(move);
	}

	// the choices to attack by hero/minion
	// TODO
}

void Board::GetPutMinionLocationMoves(std::vector<Move> &moves) const
{
	size_t total_minions = this->player_minions.GetMinions().size();
	Move move;

	moves.clear();
	moves.reserve(1 + total_minions);

	move.action = Move::ACTION_CHOOSE_PUT_MINION_LOCATION;

	for (size_t i=0; i<=total_minions; ++i) {
		move.data.choose_put_minion_location_data.put_location = i;
		moves.push_back(move);
	}
}

void Board::DoStartTurn()
{
	if (this->player_deck.GetCards().empty()) {
		// no any card can draw, take damage
		// TODO
		this->state.Set(BoardState::STAGE_WIN);
		return;
	} else {
		Card draw_card = this->player_deck.Draw();

		if (this->player_hand.GetCards().size() < 10) {
			this->player_hand.AddCard(draw_card);
		} else {
			// hand can have maximum of 10 cards
			// TODO: distroy card (trigger deathrattle?)
		}
	}

	this->state.Set(BoardState::STAGE_CHOOSE_BOARD_MOVE);
}

void Board::DoEndTurn()
{
	// TODO: change player/opponent
	this->DoStartTurn();
}

void Board::GetNextMoves(std::vector<Move> &next_moves) const
{
	switch (this->state.GetStage())
	{
		// Manage game flow
		case BoardState::STAGE_START_TURN:
		case BoardState::STAGE_END_TURN:
			this->GetGameFlowMove(next_moves);
			break;

		case BoardState::STAGE_WIN:
		case BoardState::STAGE_LOSS:
			break;

		// player/opponent's choice
		case BoardState::STAGE_CHOOSE_BOARD_MOVE:
			this->GetBoardMoves(next_moves);
			break;

		case BoardState::STAGE_CHOOSE_PUT_MINION_LOCATION:
			this->GetPutMinionLocationMoves(next_moves);
			break;

		default:
			throw new std::runtime_error("Unknown state");
	}
}

void Board::Go()
{
	switch (this->state.GetStage())
	{
		// Manage game flow
		case BoardState::STAGE_START_TURN:
			this->DoStartTurn();
			break;

		case BoardState::STAGE_END_TURN:
			this->DoEndTurn();
			break;

		default:
			throw new std::runtime_error("Unknown state for Board::Go()");
	}
}

void Board::DoPlayHandCard(const Move::PlayHandCardData &data)
{
	std::vector<Card> &hand = this->player_hand.GetCards();
	std::vector<Card>::iterator it_playing_card = hand.begin() + data.idx;

	if (it_playing_card->type == Card::MINION) {
		this->data.play_minion_data.it_hand_card = it_playing_card;
		this->state.Set(BoardState::STAGE_CHOOSE_PUT_MINION_LOCATION);
	} else {
		// TODO: other card types
		throw std::runtime_error("unknown hand card type for Board::DoPlayHandCard()");
	}
}

void Board::DoChoosePutMinionLocation(const Move::ChoosePutMinionLocationData &data)
{
	Minion minion;
	std::vector<Card>::iterator it_hand_card = this->data.play_minion_data.it_hand_card;

	// TODO: handle battlecry
	minion.card_id = it_hand_card->id;
	minion.max_hp = it_hand_card->data.minion.hp;
	minion.hp = minion.max_hp;
	minion.attack = it_hand_card->data.minion.attack;

	this->player_minions.AddMinion(minion, data.put_location);

	this->player_hand.GetCards().erase(it_hand_card);

	this->state.Set(BoardState::STAGE_CHOOSE_BOARD_MOVE);
}

void Board::ApplyMove(const Move &move)
{
	switch (move.action)
	{
		case Move::ACTION_GAME_FLOW:
			this->Go();
			break;

		case Move::ACTION_PLAY_HAND_CARD:
			this->DoPlayHandCard(move.data.play_hand_card_data);
			break;

		case Move::ACTION_CHOOSE_PUT_MINION_LOCATION:
			this->DoChoosePutMinionLocation(move.data.choose_put_minion_location_data);
			break;

		case Move::ACTION_END_TURN:
			this->state.Set(BoardState::STAGE_END_TURN);
			break;

		default:
			throw new std::runtime_error("Unknown move action");
	}
	this->last_move = move;
}

void Board::DebugPrint() const
{
	std::cout << "=== Print Board START ===" << std::endl;

	if (this->state.IsPlayerTurn()) {
		std::cout << "Player's turn." << std::endl;
	} else {
		std::cout << "Opponent's turn." << std::endl;
	}
	std::cout << "Stage: " << this->state.GetStage() << std::endl;

	std::cout << "Player deck: " << std::endl;
	std::cout << "\t";
	for (const auto &card : this->player_deck.GetCards()) {
		std::cout << card.id << " ";
	}
	std::cout << std::endl;

	std::cout << "Player hand: " << std::endl;
	std::cout << "\t";
	for (const auto &card : this->player_hand.GetCards()) {
		std::cout << card.id << " ";
	}
	std::cout << std::endl;

	std::cout << "Player minions: " << std::endl;
	this->player_minions.DebugPrint();

	std::cout << "=== Print Board END   ===" << std::endl;
}

void Move::DebugPrint() const
{
	switch (this->action)
	{
		case Move::ACTION_GAME_FLOW:
			std::cout << "[Game flow]";
			break;

		case Move::ACTION_PLAY_HAND_CARD:
			std::cout << "[Play hand card] idx = " << this->data.play_hand_card_data.idx;
			break;
		case Move::ACTION_ATTACK:
			std::cout << "[Attack] attacking = " << this->data.attack_data.attacking_idx
				<< ", attacked = " << this->data.attack_data.attacked_idx;
			break;
		case Move::ACTION_END_TURN:
			std::cout << "[End turn]";
			break;

		case Move::ACTION_CHOOSE_PUT_MINION_LOCATION:
			std::cout << "[Choose put minion location:] idx = " << this->data.choose_put_minion_location_data.put_location;
			break;

		default:
			throw std::runtime_error("unknown action for Move::DebugPrint()");
			break;
	}
}
