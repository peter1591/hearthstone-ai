#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <unordered_map>
#include <map>
#include <random>
#include <iostream>
#include <iterator>
#include <queue>
#include <unordered_set>
#include <stdexcept>

#include "game-ai.h"

#include "mcts.h"

#define EXLPORATION_FACTOR 2.0

static double CalculateSelectionWeight(TreeNode *node, double total_simulations_ln, double exploration_factor)
{
	auto const& node_win = node->wins;
	auto const& node_simulations = node->count;

	double win_rate;
	if (node->equivalent_node == nullptr) {
		// normal node
		win_rate = (double)node_win / node_simulations;
	}
	else {
		// redirect node
		win_rate = (double)node->equivalent_node->wins / node->equivalent_node->count;
	}

	double exploration_term = sqrt(total_simulations_ln / node_simulations);

	return win_rate + exploration_factor * exploration_term;
}

static TreeNode *FindBestChildToExpand(TreeNode *parent_node, double exploration_factor = EXLPORATION_FACTOR)
{
	const TreeNode::children_type &children = parent_node->children;
	const int &total_simulations = parent_node->count;

	TreeNode::children_type::const_iterator it_child = children.begin();

	double total_simulations_ln = log((double)total_simulations);

	TreeNode * max_weight_node = *it_child;

	double max_weight = CalculateSelectionWeight(max_weight_node, total_simulations_ln, exploration_factor);
	++it_child;

	for (; it_child != children.end(); ++it_child) {
		double weight = CalculateSelectionWeight(*it_child, total_simulations_ln, exploration_factor);
		if (weight > max_weight) {
			max_weight = weight;
			max_weight_node = *it_child;
		}
	}

	return max_weight_node;
}

void MCTS::Initialize(unsigned int rand_seed, const GameEngine::Board &board)
{
	srand(rand_seed);

	this->root_node_board = GameEngine::Board::Clone(board);
#ifdef DEBUG_SAVE_BOARD
	tree.GetRootNode().board = board;
#endif
	tree.GetRootNode().stage = board.GetStage();
	tree.GetRootNode().stage_type = board.GetStageType();
	tree.GetRootNode().parent = nullptr;
	tree.GetRootNode().wins = 0;
	tree.GetRootNode().count = 0;
	if (tree.GetRootNode().stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		tree.GetRootNode().is_player_node = true;
	} else {
		// Note: if the starting node is a RANDOM node,
		// then the root node's is_player_node doesn't matter
		tree.GetRootNode().is_player_node = false;
	}

	this->board_node_map.Add(board, &tree.GetRootNode());
}

void MCTS::Select(TreeNode* & node, GameEngine::Board & board)
{
#ifdef DEBUG
	if (node->equivalent_node != nullptr) throw std::runtime_error("consistency check failed");
#endif

	while (true)
	{
#ifdef DEBUG_SAVE_BOARD
		if (new_board != new_node->board) {
			throw std::runtime_error("board consistency check failed");
		}
#endif

		if (node->children.empty()) {
			return;
		}

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			return;
		}

		if (!node->next_move_getter.Empty()) {
			return;
		}

		node = FindBestChildToExpand(node);

		this->traversed_nodes.push_back(node); // back-propagate should update the node (not the redirected one)

		board.ApplyMove(node->move);

		if (node->equivalent_node != nullptr) {
			node = node->equivalent_node;
		}
	}
}

void MCTS::GetNextMove(TreeNode *node, GameEngine::Board const& board, GameEngine::Move &next_move)
{
#ifdef DEBUG_SAVE_BOARD
	if (node->board != board) {
		throw std::runtime_error("board consistency check failed");
	}
#endif

	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW)
	{
		next_move = GameEngine::Move::GetGameFlowMove(this->GetRandom());
	}
	else {
		if (node->children.empty()) {
			board.GetNextMoves(node->next_move_getter);

#ifdef DEBUG
			if (node->next_move_getter.Empty()) throw std::runtime_error("should at least return one possible move");
#endif
		}

		if (node->next_move_getter.GetNextMove(next_move) == false) {
			throw std::runtime_error("a node with no expandable move should not be selected to be expanded");
		}
	}
}

TreeNode * MCTS::FindDuplicateNode(TreeNode * node, GameEngine::Move &next_move, GameEngine::Board const& next_board, bool introduced_random)
{
	// quickly find node by move
	if (introduced_random == false && node->children.empty() == false) {
		// a deterministic node --> find in children node with 'move'
		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
#ifdef DEBUG
			if (node->children.size() != 1) throw std::runtime_error("a deterministic game-flow move should only have one outcome (i.e., one child)!");
#endif
			return node->children.front();
		}
		else {
			for (auto const& child : node->children) {
				if (child->move == next_move) return child;
			}
		}
	}

	return this->board_node_map.Find(next_board, *this);
}

// return false if 'new_node' is an expanded node
bool MCTS::Expand(TreeNode* & node, GameEngine::Board & board)
{
	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
		return true; // node cannot be expanded further (i.e., game-end)
	}

#ifdef DEBUG
	if (node->equivalent_node != nullptr) throw std::runtime_error("consistency check failed");
#endif

	auto & new_move = this->allocated_node->move;

	this->GetNextMove(node, board, new_move);

	bool introduced_random;
	board.ApplyMove(new_move, &introduced_random);
	// Note: now the 'board' is the node 'node' plus the move 'new_move'

	TreeNode *found_node = this->FindDuplicateNode(node, new_move, board, introduced_random);

	if (found_node)
	{
		this->traversed_nodes.push_back(found_node);

		bool create_redirect_node = false;

		if (found_node->parent != node) {
			// expanded before in other paths
			// --> create a redirect node
			create_redirect_node = true;
		}

		if (found_node->equivalent_node != nullptr) {
			found_node = found_node->equivalent_node;
		}

		if (create_redirect_node) {
			// check if a redirect node is already created
			bool found_redirect_node = false;
			for (auto const& child_node : node->children) {
				if (child_node->equivalent_node == found_node) {
					found_redirect_node = true;
					break;
				}
			}
			if (found_redirect_node == false) {
				TreeNode * new_node = this->allocated_node;
				this->allocated_node = new TreeNode;

				new_node->move = new_move;
				new_node->wins = 0;
				new_node->count = 0;
				new_node->equivalent_node = found_node;
				node->AddChild(new_node);
			}
		}

		node = found_node;
		return false;
	}

	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->equivalent_node = nullptr;
	new_node->move = new_move;
	new_node->wins = 0;
	new_node->count = 0;
	new_node->stage = board.GetStage();
	new_node->stage_type = board.GetStageType();
#ifdef DEBUG_SAVE_BOARD
	new_node->board = board;
#endif

	if (new_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		new_node->is_player_node = true;
	}
	else if (new_node->stage_type == GameEngine::STAGE_TYPE_OPPONENT) {
		new_node->is_player_node = false;
	}
	else {
		new_node->is_player_node = node->is_player_node; // follow the parent's status
	}

	node->AddChild(new_node);

	node = new_node;
	this->board_node_map.Add(board, node);
	this->traversed_nodes.push_back(node);

	return true;
}

bool MCTS::Simulate(GameEngine::Board &board)
{
	while (true) {
		std::vector<GameEngine::Move> next_moves;

		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			board.ApplyMove(GameEngine::Move::GetGameFlowMove(this->GetRandom()));

		} else if (stage_type == GameEngine::STAGE_TYPE_GAME_END) {
			return (board.GetStage() == GameEngine::STAGE_WIN);

		} else {
			GameEngine::Move move;
			board.GetGoodMove(move, this->GetRandom());
			board.ApplyMove(move);
		}
	}
}

void MCTS::BackPropagate(bool is_win)
{
	while (!this->traversed_nodes.empty())
	{
		TreeNode *node = this->traversed_nodes.back();
		this->traversed_nodes.pop_back();

		if (node->equivalent_node != nullptr) {
			node->count++; // only update simulation count for redirect nodes
			node = node->equivalent_node;
		}

		if (node->is_player_node) {
			if (is_win == true) node->wins++; // from the player's respect
		}
		else {
			if (is_win == false) node->wins++; // from the opponent's respect
		}
		node->count++;
	}
}

void MCTS::Iterate()
{
	TreeNode *node = &this->tree.GetRootNode();
	GameEngine::Board board = GameEngine::Board::Clone(this->root_node_board);

#ifdef DEBUG
	if (!this->traversed_nodes.empty()) throw std::runtime_error("consistency check failed");
#endif

	this->traversed_nodes.push_back(node);

	// loop if expand a duplicated node
	while (true) {
		this->Select(node, board);
		if (this->Expand(node, board)) break;
	}

	bool is_win = this->Simulate(board);
	this->BackPropagate(is_win);
}

MCTS::MCTS()
{
	this->allocated_node = new TreeNode;
}

MCTS::~MCTS()
{
	if (this->allocated_node) delete this->allocated_node;
}

MCTS::MCTS(MCTS&& rhs)
{
	*this = std::move(rhs);
}

MCTS & MCTS::operator=(MCTS&& rhs)
{
	this->root_node_board = std::move(rhs.root_node_board);
	this->tree = std::move(rhs.tree);
	this->board_node_map = std::move(rhs.board_node_map);

	this->allocated_node = std::move(rhs.allocated_node);
	this->traversed_nodes = std::move(rhs.traversed_nodes);

	return *this;
}

bool MCTS::operator==(const MCTS& rhs) const
{
	if (this->root_node_board != rhs.root_node_board) return false;
	if (this->tree != rhs.tree) return false;

	// skip checking for internal consistency data
	// if (this->board_node_map != rhs.board_node_map) return false;

	return true;
}

bool MCTS::operator!=(const MCTS& rhs) const
{
	return !(*this == rhs);
}

int MCTS::GetRandom()
{
	return rand();
}