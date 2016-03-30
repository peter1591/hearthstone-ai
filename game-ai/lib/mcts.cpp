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

#include "game-ai/game-ai.h"

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

void MCTS::Initialize(unsigned int rand_seed, StartBoard && start_board)
{
	srand(rand_seed);
	this->start_board = std::move(start_board);
}

bool MCTS::UseNextMoveGetter(TreeNode * node)
{
	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) return false;
	else return true;
}

bool MCTS::ExpandNewNode(TreeNode * & node, GameEngine::Board & board)
{
	GameEngine::Move & expanding_move = this->allocated_node->move;

	if (this->UseNextMoveGetter(node)) {
		board.GetNextMoves(node->next_move_getter);
		if (node->next_move_getter.GetNextMove(expanding_move) == false) {
			throw std::runtime_error("should at least return one possible move");
		}
		node->next_moves_are_random = false; // currently the next-move-getter is deterministic
	}
	else {
		board.GetNextMoves(this->GetRandom(), expanding_move, &node->next_moves_are_random);
	}

	bool next_board_is_random;
	board.ApplyMove(expanding_move, &next_board_is_random);

	// find transposition node (i.e., other node with the same board)
	TreeNode *transposition_node = this->board_node_map.Find(board, this->current_iteration_root_node_board);
	if (transposition_node) {
		// a transposition node is found
#ifdef DEBUG
		if (transposition_node->parent == node) throw std::runtime_error("logic error: 'node' should have no parent in ExpandNewNode()");
		if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");
#endif

		TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, transposition_node);
		this->traversed_nodes.push_back(redirect_node);

		node = transposition_node;
		return false;
	}

	node = this->CreateChildNode(node, expanding_move, board);
	return true;
}

bool MCTS::ExpandNodeWithDeterministicNextMoves(TreeNode * & node, GameEngine::Board & board)
{
	// next moves are deterministic, and they are stored in next_move_getter

	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	if (!this->UseNextMoveGetter(node)) throw std::runtime_error("game-flow stages should all with non-deterministic next moves");
#endif

	if (node->next_move_getter.GetNextMove(expanding_move)) {
		// not fully expanded yet

#ifdef DEBUG
		for (auto const& child : node->children) {
			if (child->move == expanding_move) {
				throw std::runtime_error("next-move-getter returns one particular move twice!");
			}
		}
#endif

		bool next_board_is_random;
		board.ApplyMove(expanding_move, &next_board_is_random);

		TreeNode *transposition_node = this->board_node_map.Find(board, this->current_iteration_root_node_board);
		if (transposition_node) {
#ifdef DEBUG
			if (transposition_node->parent == node) {
				// expanded before under the same parent but with different move
				// --> no need to create a new redirect node
				throw std::runtime_error("two different moves under the same parent yield identical game states, why is this happening?");
				// Note: we can still create redirect node, but we want to know why this is happening
			}
			if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");

			for (auto const& child_node : node->children) {
				if (child_node->equivalent_node == transposition_node) {
					throw std::runtime_error("two different moves yield an identical board, why is this happening?");
				}
			}
#endif

			// create a redirect node
			TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, transposition_node);
			this->traversed_nodes.push_back(redirect_node);

			node = transposition_node;
			return false;
		}

		node = this->CreateChildNode(node, expanding_move, board);
		return true;
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
bool MCTS::ExpandNodeWithSingleRandomNextMove(TreeNode * & node, GameEngine::Board & board)
{
	// we only have one possible next move,
	// so we don't need to create redirect nodes
	// Note: redirect nodes are important in the MCTS selection phase,
	//       however, since we only have one possible next move,
	//       we re-generate a new game-flow move, and follow it

	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	bool prev_next_moves_are_random = node->next_moves_are_random;
#endif
	board.GetNextMoves(this->GetRandom(), expanding_move, &node->next_moves_are_random);
#ifdef DEBUG
	if (node->next_moves_are_random != prev_next_moves_are_random) {
		throw std::runtime_error("parent->next_moves_are_random should not be altered.");
	}
#endif

	bool next_board_is_random;
	board.ApplyMove(expanding_move, &next_board_is_random);

	TreeNode * transposition_node = nullptr;

	if (next_board_is_random == false) {
		// the move is applied in a deterministic way
		// --> that is, all moves are deterministic
		// --> so, we have only one outcome

#ifdef DEBUG
		if (node->children.empty()) throw std::runtime_error("You should call ExpandNewNode() to expand at least once.");
		if (node->children.size() != 1) throw std::runtime_error("a deterministic game-flow move should only have one outcome (i.e., one child)!");
#endif

		transposition_node = node->children.front();

		this->traversed_nodes.push_back(transposition_node);
		if (transposition_node->equivalent_node != nullptr) {
			transposition_node = transposition_node->equivalent_node;
		}
		node = transposition_node;
		return false;
	}

	// game-flow move is non-determinsitic for this board
	transposition_node = this->board_node_map.Find(board, this->current_iteration_root_node_board);
	if (transposition_node) {
#ifdef DEBUG
		if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");
#endif

		this->traversed_nodes.push_back(transposition_node);
		node = transposition_node;
		return false;
	}

	node = this->CreateChildNode(node, expanding_move, board);
	return true;
}

// return true if a new node is added; 'node' and 'board' will be the new node
// return false if an existing node is chosen; 'node' and 'board' will be the existing node
bool MCTS::ExpandNodeWithMultipleRandomNextMoves(TreeNode * & node, GameEngine::Board & board)
{
	throw std::runtime_error("currently only stages with only one next move are supported if the stage's next moves are random.");
}

void MCTS::SelectAndExpand(TreeNode* & node, GameEngine::Board & board)
{
	while (true)
	{
#ifdef DEBUG
		if (node->equivalent_node != nullptr) throw std::runtime_error("consistency check failed");
		size_t current_hash = std::hash<GameEngine::Board>()(board);
		if (node->board_hash != current_hash) throw std::runtime_error("consistency check failed: board hash changed");
#endif

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) return;

		if (node->children.empty()) {
			// not expanded before
			if (this->ExpandNewNode(node, board)) return;
		}
		else if (node->next_moves_are_random) {
			if (!this->UseNextMoveGetter(node)) {
				// quick process if we have only one next move
				if (this->ExpandNodeWithSingleRandomNextMove(node, board)) return;
			}
			else {
				if (this->ExpandNodeWithMultipleRandomNextMoves(node, board)) return;
			}
		}
		else {
			if (this->ExpandNodeWithDeterministicNextMoves(node, board)) return;
		}
	}
}

TreeNode * MCTS::CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node)
{
	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->move = move;
	new_node->wins = 0;
	new_node->count = 0;
	new_node->equivalent_node = target_node;
	parent->AddChild(new_node);

	return new_node;
}

// return true if a new (normal) node is created; 'new_node' will be the created new child
// return false is a redirect node is created; 'new_node' will be the target node
TreeNode * MCTS::CreateChildNode(TreeNode* const node, GameEngine::Move const& next_move, GameEngine::Board & next_board)
{
	// Note: now the 'next_board' is the node 'node' plus the move 'next_move'

	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->equivalent_node = nullptr;
#ifdef DEBUG
	if (&new_node->move != &next_move) throw std::runtime_error("we've optimized by assuming next_move === this->allocated_node->move");
	//new_node->move = expanding_move; // optimized out
#endif
	new_node->wins = 0;
	new_node->count = 0;
	new_node->stage = next_board.GetStage();
	new_node->stage_type = next_board.GetStageType();

	if (new_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		new_node->is_player_node = true;
	}
	else if (new_node->stage_type == GameEngine::STAGE_TYPE_OPPONENT) {
		new_node->is_player_node = false;
	}
	else {
		new_node->is_player_node = node->is_player_node; // follow the parent's status
	}

#ifdef DEBUG
	new_node->board_hash = std::hash<GameEngine::Board>()(next_board);
#endif

	node->AddChild(new_node);

	this->board_node_map.Add(next_board, new_node);
	this->traversed_nodes.push_back(new_node);

	return new_node;
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

void MCTS::CreateRootNode()
{
	tree.CreateRootNode();
	TreeNode * root_node = tree.GetRootNode();

	root_node->stage = this->current_iteration_root_node_board.GetStage();
	root_node->stage_type = this->current_iteration_root_node_board.GetStageType();
	root_node->parent = nullptr;
	root_node->wins = 0;
	root_node->count = 0;
	if (root_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		root_node->is_player_node = true;
	}
	else {
		// Note: if the starting node is a RANDOM node,
		// then the root node's is_player_node doesn't matter
		root_node->is_player_node = false;
	}
#ifdef DEBUG
	root_node->board_hash = std::hash<GameEngine::Board>()(this->current_iteration_root_node_board);
#endif

	this->board_node_map.Add(this->current_iteration_root_node_board, root_node);
}

void MCTS::Iterate()
{
	int current_rand = rand();
	this->current_iteration_root_node_board = this->start_board.GetBoard(current_rand);

	if (!this->tree.GetRootNode()) {
		this->CreateRootNode();
	}

	TreeNode *node = this->tree.GetRootNode();
	GameEngine::Board board = GameEngine::Board::Clone(this->current_iteration_root_node_board);

#ifdef DEBUG
	if (!this->traversed_nodes.empty()) throw std::runtime_error("consistency check failed");
#endif

	this->traversed_nodes.push_back(node);

	this->SelectAndExpand(node, board);
	bool is_win = this->Simulate(board);
	this->BackPropagate(is_win);
}

Tree const & MCTS::GetTree() const
{
	return this->tree;
}

MCTS::MCTS()
{
	this->allocated_node = new TreeNode;
}

MCTS::~MCTS()
{
	if (this->allocated_node) delete this->allocated_node;
}

int MCTS::GetRandom()
{
	return rand();
}