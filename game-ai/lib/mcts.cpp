#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <iostream>
#include <stdexcept>

#include "mcts.h"

#define EXLPORATION_FACTOR 5000000.0

static double CalculateSelectionWeight(
		int node_win, int node_simulations, double total_simulations_ln, double exploration_factor)
{
	double win_rate = (double)node_win / node_simulations;
	double exploration_term = sqrt(total_simulations_ln / node_simulations);

	return -node_simulations;
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

	this->traversed_boards[board] = &tree.GetRootNode();
}

// Find a node to expand, and expand it
// @param board [OUT] the new board of the node
// @return the new node
TreeNode * MCTS::SelectAndExpand(GameEngine::Board &board)
{
	TreeNode *expanding_node = &this->tree.GetRootNode();
	board = this->root_node_board;
	TreeNode *new_node = new TreeNode;

	// Find a node to expand, and expand it.
	// When loop exits:
	//    'new_node' is expanded from 'expanding_node' with move 'new_node->move'
	while (true) {
		// Loop invariance:
		//    'board' is the board of the node 'expanding_node'
#ifdef CHECK_MOVE_REAPPLIABLE
		if (board != expanding_node->board) {
			throw std::runtime_error("board consistency check failed");
		}
#endif

		// if it's a leaf
		if (expanding_node->children.empty()) {
			if (expanding_node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
				return expanding_node;

			} else if (expanding_node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
				new_node->move = GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed));
				board.ApplyMove(new_node->move);
				break;

			} else {
				board.GetNextMoves(expanding_node->moves_not_yet_expanded);
				new_node->move = expanding_node->moves_not_yet_expanded.back();
				board.ApplyMove(new_node->move);
				expanding_node->moves_not_yet_expanded.pop_back();
				break;
			}
		}

#ifdef DEBUG
		if (expanding_node->count == 0) {
			throw std::runtime_error("tree node has children, but simulation count is zero");
		}
#endif

		if (expanding_node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			// apply the game-flow move
			new_node->move = GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed));
			board.ApplyMove(new_node->move);

			decltype(this->traversed_boards)::const_iterator it = this->traversed_boards.find(board);
			if (it != this->traversed_boards.end()) {
				// this node has been expanded before --> select the node in its sub-tree
				expanding_node = it->second;
				continue;
			} else {
				break; // this is a new node
			}

		} else {
#ifdef DEBUG
			if (expanding_node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
				throw std::runtime_error("a game-end node should not has children nodes");
			}
#endif

			if (expanding_node->moves_not_yet_expanded.empty()) {
				expanding_node = FindBestChildToExpand(expanding_node);
				if (expanding_node->parent == &this->tree.GetRootNode()) {
				//	std::cout << "expanding root node, found node with count " << expanding_node->count << std::endl;
				}
				board.ApplyMove(expanding_node->move);
				continue;

			} else {
				// some node has not yet been expanded, expand it
				new_node->move = expanding_node->moves_not_yet_expanded.back();
				board.ApplyMove(new_node->move);
				expanding_node->moves_not_yet_expanded.pop_back();
				break;
			}
		}
	}

#ifdef CHECK_MOVE_REAPPLIABLE
	new_node->board = board;
#endif
	new_node->stage = board.GetStage();
	new_node->stage_type = board.GetStageType();

	if (new_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		new_node->is_player_node = true;
	} else if (new_node->stage_type == GameEngine::STAGE_TYPE_OPPONENT) {
		new_node->is_player_node = false;
	} else {
		new_node->is_player_node = expanding_node->is_player_node; // follow the parent's status
	}

	expanding_node->AddChild(new_node);
	this->traversed_boards[board] = new_node;

	return new_node;
}

void MCTS::SimulateWithBoard(GameEngine::Board &board)
{
	while (true) {
		std::vector<GameEngine::Move> next_moves;

		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			board.ApplyMove(GameEngine::Move::GetGameFlowMove(rand_r(&this->rand_seed)));

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
			board.ApplyMove(next_moves[choose_move]);
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

static TreeNode *FindBestChildToPlay(TreeNode *node)
{
	if (node->children.empty()) throw std::runtime_error("failed");

	TreeNode *most_simulated = node->children.front();
	for (const auto &child: node->children) {
		if (child->count > most_simulated->count) {
			most_simulated = child;
		}
	}

	return most_simulated;
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

		//node = FindBestChildToPlay(node);
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

	PrintTree(&this->tree.GetRootNode(), 0, 2);
	//PrintBestRoute();
}
