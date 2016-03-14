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

static TreeNode *FindBestChildToExpand(
	TreeNode *parent_node,
	double exploration_factor = EXLPORATION_FACTOR,
	std::function<bool(TreeNode*)> * is_valid_child = nullptr)
{
	const TreeNode::children_type &children = parent_node->children;
	const int &total_simulations = parent_node->count;

	TreeNode::children_type::const_iterator it_child = children.begin();

	double total_simulations_ln = log((double)total_simulations);

	TreeNode * max_weight_node = nullptr;

	double max_weight = -std::numeric_limits<double>::max();

	for (; it_child != children.end(); ++it_child) {
		if (is_valid_child && (*is_valid_child)(*it_child)) continue;

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

bool MCTS::ExpandNewNode(TreeNode * & node, GameEngine::Board & board)
{
	GameEngine::Move & expanding_move = this->allocated_node->move;

	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
		board.GetNextMoves(this->GetRandom(), expanding_move, &node->next_moves_are_random);
	}
	else {
		board.GetNextMoves(node->next_move_getter);
		if (node->next_move_getter.GetNextMove(expanding_move) == false) {
			throw std::runtime_error("should at least return one possible move");
		}
		node->next_moves_are_random = false; // currently the next-move-getter is deterministic
	}

	TreeNode * new_node;
	bool ret = this->Expand(node, board, expanding_move, new_node);
	node = new_node;
	return ret;
}

bool MCTS::ExpandNodeWithDeterministicMoves(TreeNode * & node, GameEngine::Board & board)
{
	// next moves are deterministic, and they are stored in next_move_getter

	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) throw std::runtime_error("game-flow stages should all with non-deterministic next moves");
#endif

	if (node->next_move_getter.GetNextMove(expanding_move)) {
		// not fully expanded yet
		TreeNode * new_node;
		bool ret = this->Expand(node, board, expanding_move, new_node);
		node = new_node;
		return ret;
	}

	node = ::FindBestChildToExpand(node);
#ifdef DEBUG
	if (node == nullptr) throw std::runtime_error("cannot find best child to expand");
#endif

	this->traversed_nodes.push_back(node); // back-propagate need to know the original node (not the redirected one)

	board.ApplyMove(node->move);

	if (node->equivalent_node != nullptr) {
		node = node->equivalent_node;
	}

	return false;
}

// return true if a new node is added; 'node' and 'board' will be the new node
// return false if an existing node is chosen; 'node' and 'board' will be the existing node
bool MCTS::ExpandNodeWithRandomMoves(TreeNode * & node, GameEngine::Board & board)
{
	GameEngine::Move & expanding_move = this->allocated_node->move;

	// next moves are non-deterministic
	// --> get new next moves
	// --> expand them
	// we done these in Expand()

#ifdef DEBUG
	if (node->stage_type != GameEngine::STAGE_TYPE_GAME_FLOW) throw std::runtime_error("currently only game-flow stages are supported with non-deterministic next moves.");
#endif

#ifdef DEBUG
	bool prev_next_moves_are_random = node->next_moves_are_random;
#endif
	board.GetNextMoves(this->GetRandom(), expanding_move, &node->next_moves_are_random);
#ifdef DEBUG
	if (node->next_moves_are_random != prev_next_moves_are_random) {
		throw std::runtime_error("parent->next_moves_are_random should not be altered.");
	}
#endif

	TreeNode * new_node;
	bool ret = this->Expand(node, board, expanding_move, new_node);
	node = new_node;
	return ret;
}

void MCTS::SelectAndExpand(TreeNode* & node, GameEngine::Board & board)
{
	while (true)
	{
#ifdef DEBUG
		if (node->equivalent_node != nullptr) throw std::runtime_error("consistency check failed");
#endif

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) return;

		if (node->children.empty()) {
			// not expanded before
			if (this->ExpandNewNode(node, board)) return;
		}
		else if (node->next_moves_are_random) {
			if (this->ExpandNodeWithRandomMoves(node, board)) return;
		}
		else {
			if (this->ExpandNodeWithDeterministicMoves(node, board)) return;
		}
	}
}

TreeNode * MCTS::FindDuplicateNode(TreeNode * node, GameEngine::Move const& next_move, GameEngine::Board const& next_board, bool introduced_random)
{
	// quickly find node by move
	if (introduced_random == false && node->children.empty() == false) {
		// a deterministic node --> find in children node with 'next_move'
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

	return nullptr;
}

TreeNode * MCTS::CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node)
{
	// check if a redirect node is already created
	for (auto const& child_node : parent->children) {
		if (child_node->equivalent_node == target_node) {
			return child_node;
		}
	}

	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->move = move;
	new_node->wins = 0;
	new_node->count = 0;
	new_node->equivalent_node = target_node;
	parent->AddChild(new_node);

	return new_node;
}

// return false if 'new_node' is an expanded node
bool MCTS::Expand(TreeNode* const node, GameEngine::Board & board, GameEngine::Move const& expanding_move, TreeNode* & new_node)
{
	bool next_board_is_random;
	board.ApplyMove(expanding_move, &next_board_is_random);
	// Note: now the 'board' is the node 'node' plus the move 'new_move'

	TreeNode *found_node = this->FindDuplicateNode(node, expanding_move, board, next_board_is_random);

	if (found_node == nullptr) found_node = this->board_node_map.Find(board, *this);

	if (found_node)
	{
		if (found_node->parent == node) {
			// expanded before in the same parent --> no need to create a new redirect node
			this->traversed_nodes.push_back(found_node);
			if (found_node->equivalent_node != nullptr) {
				found_node = found_node->equivalent_node;
			}
		}
		else {
			// expanded before in other paths --> create a redirect node
			if (found_node->equivalent_node != nullptr) {
				found_node = found_node->equivalent_node;
			}
			TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, found_node);
			this->traversed_nodes.push_back(redirect_node);
		}

		new_node = found_node;
		return false;
	}

	new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->equivalent_node = nullptr;
#ifdef DEBUG
	if (&new_node->move != &expanding_move) throw std::runtime_error("it should be");
	//new_node->move = expanding_move; // we've already done this, since new_move is a reference to new_node->move
#endif
	new_node->wins = 0;
	new_node->count = 0;
	new_node->stage = board.GetStage();
	new_node->stage_type = board.GetStageType();

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

	this->board_node_map.Add(board, new_node);
	this->traversed_nodes.push_back(new_node);

	return true;
}

bool MCTS::Simulate(GameEngine::Board &board)
{
	while (true) {
		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			GameEngine::Move next_move;
			board.GetNextMoves(this->GetRandom(), next_move);
			board.ApplyMove(next_move);

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

	this->SelectAndExpand(node, board);
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