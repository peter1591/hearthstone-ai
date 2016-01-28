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

#include "mcts.h"

#define EXLPORATION_FACTOR 1.0

static double CalculateSelectionWeight(
		int node_win, int node_simulations, double total_simulations_ln, double exploration_factor)
{
	double win_rate = (double)node_win / node_simulations;
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

	double max_weight = CalculateSelectionWeight(max_weight_node->wins, max_weight_node->count, total_simulations_ln, exploration_factor);

	for (; it_child != children.end(); ++it_child) {
		double weight = CalculateSelectionWeight((*it_child)->wins, (*it_child)->count, total_simulations_ln, exploration_factor);
		if (weight > max_weight) {
			max_weight = weight;
			max_weight_node = *it_child;
		}
	}

	return max_weight_node;
}

void MCTS::Initialize(unsigned int rand_seed, const GameEngine::Board &board)
{
	this->random_generator.seed(rand_seed);

	this->root_node_board = board;
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

	this->board_node_map.Add(board, &tree.GetRootNode(), *this);
}

TreeNode * MCTS::Select(TreeNode *node, GameEngine::Board &board)
{
	while (true)
	{
#ifdef DEBUG_SAVE_BOARD
		if (board != node->board) {
			throw std::runtime_error("board consistency check failed");
		}
#endif

		if (node->children.empty()) {
			return node;
		}

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			return node;
		}

		if (node->moves_not_yet_expanded.empty()) {
			node = FindBestChildToExpand(node);
			board.ApplyMove(node->move);
			continue;
		}
		else {
			return node;
		}
	}
}

void MCTS::Expand(TreeNode *node, GameEngine::Move &move, GameEngine::Board &board)
{
#ifdef DEBUG
	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
		throw std::runtime_error("a game-end stage should not be expanded");
	}
#endif

#ifdef DEBUG_SAVE_BOARD
	if (node->board != board) {
		throw std::runtime_error("board consistency check failed");
	}
#endif

	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW)
	{
		move = GameEngine::Move::GetGameFlowMove(this->GetRandom());
		board.ApplyMove(move);
	}
	else {
		if (node->children.empty()) {
			board.GetNextMoves(node->moves_not_yet_expanded);
		}

#ifdef DEBUG
		if (node->moves_not_yet_expanded.empty()) {
			throw std::runtime_error("a node with no non-expanded move should not be selected to be expanded");
		}
#endif

		move = node->moves_not_yet_expanded.back();
		node->moves_not_yet_expanded.pop_back();
		board.ApplyMove(move);
	}
}

// if the board has not been traversed, return nullptr
// if the board has been traversed, return the pointer to the child node
TreeNode* MCTS::IsBoardTraversed(TreeNode *parent, const GameEngine::Board new_child_board)
{
	if (parent->stage_type != GameEngine::STAGE_TYPE_GAME_FLOW) {
		// always create a new node for non-random (i.e., non-game-flow) nodes
		// since it must just expanded by a new move in Expand()
		return nullptr;
	}

	// check if random-generated game flow move has already expanded before
	auto it_possible_nodes = this->board_node_map.Find(new_child_board, *this);
	for (const auto &it_possible_node : it_possible_nodes) {
		if (it_possible_node->parent == parent) {
			return it_possible_node;
		}
	}

	return nullptr; // not found
}

// Find a node to expand, and expand it
TreeNode * MCTS::SelectAndExpand(GameEngine::Board &board)
{
	TreeNode *new_node = new TreeNode;
	TreeNode *node = &this->tree.GetRootNode();
	board = this->root_node_board;

	while (true)
	{
		node = this->Select(node, board);

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
			delete new_node;
			return node;
		}

		this->Expand(node, new_node->move, board);

		TreeNode *found_node = this->IsBoardTraversed(node, board);
		if (found_node == nullptr) {
			break; // create a new node
		}
		else {
			node = found_node; // select again among that subtree
		}
	}

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

	board_node_map.Add(board, new_node, *this);

	return new_node;
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
			board.GetNextMoves(next_moves);

#ifdef DEBUG
			if (next_moves.empty()) {
				throw std::runtime_error("stage type is not GAME_END, but no next move is available");
			}
#endif

			// do random move here
			int choose_move = this->GetRandom() % next_moves.size();
			board.ApplyMove(next_moves[choose_move]);
		}
	}
}

void MCTS::BackPropagate(TreeNode *starting_node, bool is_win)
{
	// find all leaf nodes with the same board
	// this simulation can also be seen as simulated from these leaf nodes
	// so we also update them

	GameEngine::Board board;
	starting_node->GetBoard(this->root_node_board, board);

	auto updating_leaf_nodes = this->board_node_map.Find(board, *this);
	for (const auto &updating_leaf_node : updating_leaf_nodes) {
		TreeNode *node = updating_leaf_node;
		while (node != nullptr) {
			if (node->is_player_node) {
				if (is_win == true) node->wins++; // from the player's respect
			}
			else {
				if (is_win == false) node->wins++; // from the opponent's respect
			}
			node->count++;
			node = node->parent;
		}
	}
}

void MCTS::Iterate()
{
	GameEngine::Board board;

	TreeNode *node = this->SelectAndExpand(board);
	bool is_win = this->Simulate(board);
	this->BackPropagate(node, is_win);
}

MCTS::MCTS()
{
}

MCTS::MCTS(const MCTS& rhs)
{
	*this = rhs;
}

MCTS::MCTS(MCTS&& rhs)
{
	*this = std::move(rhs);
}

MCTS & MCTS::operator=(const MCTS& rhs)
{
	std::unordered_map<TreeNode *, TreeNode *> node_update_map;
	auto node_update_callback = [&node_update_map](TreeNode *origin_pointer, TreeNode *new_pointer) {
		node_update_map.insert(std::make_pair(origin_pointer, new_pointer));
	};

	this->root_node_board = rhs.root_node_board;
	this->tree = rhs.tree.Clone(node_update_callback);
	this->board_node_map = rhs.board_node_map;
	this->board_node_map.UpdateNodePointers(node_update_map);
	this->random_generator = rhs.random_generator;

	return *this;
}

MCTS & MCTS::operator=(MCTS&& rhs)
{
	this->root_node_board = std::move(rhs.root_node_board);
	this->tree = std::move(rhs.tree);
	this->board_node_map = std::move(rhs.board_node_map);
	this->random_generator = std::move(rhs.random_generator);
	return *this;
}

bool MCTS::operator==(const MCTS& rhs) const
{
	if (this->root_node_board != rhs.root_node_board) return false;
	if (this->tree != rhs.tree) return false;

	// skip checking for internal consistency data
	// if (this->board_node_map != rhs.board_node_map) return false;

	if (this->random_generator != rhs.random_generator) return false;

	return true;
}

bool MCTS::operator!=(const MCTS& rhs) const
{
	return !(*this == rhs);
}

int MCTS::GetRandom()
{
	return this->random_generator();
}