#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <iostream>
#include <stdexcept>

#include "mcts.h"

#define EXLPORATION_FACTOR 0.1

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

TreeNode * MCTS::Select()
{
	TreeNode *parent_node = &this->tree.GetRootNode();

	while (true)
	{
		// loop until we reach a leaf
		if (parent_node->children.empty()) {
			break;
		}

#ifdef DEBUG
		if (parent_node->count == 0) {
			throw std::runtime_error("tree node has children, but simulation count is zero");
		}
#endif

		if (parent_node->has_random_playouts) {
			// TODO: a fixed ratio to expand a random node again?
			if (parent_node->count % 4 != 0) {
				break; // TODO: 75% chance to re-expand a random node
			}

		} else if (parent_node->moves_not_yet_expanded.empty() == false) {
			break; // some node has not yet been expanded, expand it
		}

		parent_node = FindBestChildToExpand(parent_node);
	}

	return parent_node;
}

void MCTS::Initialize(const GameEngine::Board &board)
{
	this->rand_seed = time(NULL);

	this->root_node_board = board;
#ifdef CHECK_MOVE_REAPPLIABLE
	tree.GetRootNode().board = board;
#endif
	tree.GetRootNode().stage = board.GetStage();
	tree.GetRootNode().stage_type = board.GetStageType();
	tree.GetRootNode().parent = nullptr;
	tree.GetRootNode().count = 0;
	if (tree.GetRootNode().stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		tree.GetRootNode().is_player_node = true;
	} else {
		// Note: if the starting node is a RANDOM node,
		// then the root node's is_player_node doesn't matter
		tree.GetRootNode().is_player_node = false;
	}
}

void MCTS::GetBoardByApplyingMoves(TreeNode *parent, GameEngine::Board &board) const
{
	bool dummy;

	if (parent->parent == nullptr) {
		board = this->root_node_board;
		return;
	}
	this->GetBoardByApplyingMoves(parent->parent, board);

	board.ApplyMove(parent->move, dummy);

#ifdef CHECK_MOVE_REAPPLIABLE
	if (board != parent->board) {
		throw std::runtime_error("Game engine error!!! Move is not reappliable!");
	}
#endif
}

TreeNode *MCTS::ExpandNodeWithMove(TreeNode *parent, const GameEngine::Move &move, GameEngine::Board &new_board)
{
	bool is_deterministic;
	TreeNode *new_node = new TreeNode;

	new_node->move = move; // TODO: we can use move operator
	new_board.ApplyMove(move, is_deterministic);

	if (parent->children.empty()) {
		parent->has_random_playouts = !is_deterministic;
#ifdef DEBUG
	} else {
		if (parent->has_random_playouts != (!is_deterministic)) {
			throw std::runtime_error("node previously report it's a deterministic node, but now reports not");
		}
#endif
	}

#ifdef CHECK_MOVE_REAPPLIABLE
	new_node->board = new_board;
#endif
	new_node->stage = new_board.GetStage();
	new_node->stage_type = new_board.GetStageType();

	if (new_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		new_node->is_player_node = true;
	} else if (new_node->stage_type == GameEngine::STAGE_TYPE_OPPONENT) {
		new_node->is_player_node = false;
	} else {
		new_node->is_player_node = parent->is_player_node; // follow the parent's status
	}

	parent->AddChild(new_node);

	return new_node;
	
}

TreeNode * MCTS::Expand(TreeNode *node, GameEngine::Board &new_board)
{
	TreeNode *new_node;

	// get node's board
	this->GetBoardByApplyingMoves(node, new_board);

	// not expanded yet
	if (node->children.empty()) {
		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			new_node = this->ExpandNodeWithMove(node, GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed)), new_board);
		} else {
			new_board.GetNextMoves(node->moves_not_yet_expanded);
			new_node = this->ExpandNodeWithMove(node, node->moves_not_yet_expanded.back(), new_board);
			node->moves_not_yet_expanded.pop_back();
		}

	// expanded previously
	} else {
		if (node->moves_not_yet_expanded.empty()) {
#ifdef DEBUG
			if (node->stage_type != GameEngine::STAGE_TYPE_GAME_FLOW) {
				throw std::runtime_error("This is not a game-flow stage, and all moves are already expanded. So this node should not be selected by MSTC selection phase.");
			}
#endif

			new_node = this->ExpandNodeWithMove(node, GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed)), new_board);

		} else {
			new_node = this->ExpandNodeWithMove(node, node->moves_not_yet_expanded.back(), new_board);
			node->moves_not_yet_expanded.pop_back();
		}
	}

	return new_node;
}

void MCTS::SimulateWithBoard(GameEngine::Board &board)
{
	while (true) {
		bool is_deterministic; // dummy
		std::vector<GameEngine::Move> next_moves;

		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			board.ApplyMove(GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed)), is_deterministic);

		} else if (stage_type == GameEngine::STAGE_TYPE_GAME_END) {
			return;

		} else {
			board.GetNextMoves(next_moves);

#ifdef DEBUG
			if (next_moves.empty()) {
				throw std::runtime_error("stage type is not GAME_END, but no next move is available");
			}
#endif

			// do random move here
			int choose_move = rand() % next_moves.size();
			board.ApplyMove(next_moves[choose_move], is_deterministic);
		}
	}
}

// return true if win; false otherwise
bool MCTS::Simulate(GameEngine::Board &board)
{
	this->SimulateWithBoard(board);

#ifdef DEBUG
	if (board.GetStageType() != GameEngine::STAGE_TYPE_GAME_END) {
		throw std::runtime_error("Simulate should stop at game-end stage");
	}
#endif

	return (board.GetStage() == GameEngine::STAGE_WIN);
}

void MCTS::BackPropagate(TreeNode *node, bool is_win)
{
	while (node != nullptr) {
		if (node->is_player_node) {
			if (is_win == true) node->wins++; // from the player's respect
		} else {
			if (is_win == false) node->wins++; // from the opponent's respect
		}
		node->count++;
		node = node->parent;
	}
}

static void PrintLevelPrefix(int level)
{
	for (int i=0; i<level; i++) {
		std::cout << "..";
	}
}

void MCTS::PrintTree(TreeNode *node, int level, const int max_level)
{
	if (level > max_level) return;

	PrintLevelPrefix(level);
	std::cout << "[" << node->stage << "] ";

	if (node != &this->tree.GetRootNode()) {
		std::cout << node->move.GetDebugString();
	}
	std::cout << " " << node->wins << "/" << node->count << std::endl;
	for (const auto &child: node->children) {
		PrintTree(child, level+1, max_level);
	}
}

void MCTS::PrintBestRoute()
{
	TreeNode *node = &this->tree.GetRootNode();

	int level = 0;
	while (!node->children.empty()) {
		PrintLevelPrefix(level);
		std::cout << "[" << node->stage << "] ";
		if (node != &this->tree.GetRootNode()) {
			std::cout << node->move.GetDebugString();
		}
		std::cout << " " << node->wins << "/" << node->count << std::endl;

		node = FindBestChildToExpand(node, 0.0);

		level++;
	}
}

static void TotalTreeNodes(TreeNode *node, int &total)
{
	if (node == nullptr) return;
	total++;
	for (const auto &child : node->children) {
		TotalTreeNodes(child, total);
	}
}

void MCTS::DebugPrint()
{
	int tree_nodes = 0;
	int tree_nodes_size = 0;
	TotalTreeNodes(&this->tree.GetRootNode(), tree_nodes);
	std::cout << "Total nodes: " << tree_nodes << std::endl;

	tree_nodes_size = tree_nodes * sizeof(TreeNode);
	std::cout << "Estimate tree nodes size: " << tree_nodes_size << std::endl;

	//PrintTree(&this->tree.GetRootNode(), 0, 4);
	//PrintBestRoute();
}
