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

	this->board_nodes_mapping.Add(board, &tree.GetRootNode(), *this);
}

TreeNode * MCTS::Select(TreeNode *node, GameEngine::Board &board)
{
	while (true)
	{
#ifdef CHECK_MOVE_REAPPLIABLE
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

#ifdef CHECK_MOVE_REAPPLIABLE
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

// Find a node to expand, and expand it
// @param board [OUT] the new board of the node
// @return the new node
TreeNode * MCTS::SelectAndExpand(GameEngine::Board &board)
{
	TreeNode *new_node = new TreeNode;

	board = this->root_node_board;
	TreeNode *node = this->Select(&this->tree.GetRootNode(), board);

	while (true)
	{
		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
			delete new_node;
			return node;
		}

		this->Expand(node, new_node->move, board);

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			// check if random-generated game flow move has already expanded before
			auto it_possible_nodes = this->board_nodes_mapping.Find(board, *this);
			TreeNode *found_node = nullptr;
			for (const auto &it_possible_node : it_possible_nodes) {
				if (it_possible_node->parent == node) {
					found_node = it_possible_node;
					break;
				}
			}

			if (found_node != nullptr) {
				// found --> this board is found via the parent node 'node' before
				// --> select among that child node
				node = this->Select(found_node, board);
				continue;
			}
			else {
				// not found --> this board is not found via the parent node 'node' before
				// --> make a new node
				break;
			}
		}
		else {
			break; // always create a new node for non-random (i.e., non-game-flow) nodes
		}
	}

#ifdef CHECK_MOVE_REAPPLIABLE
	new_node->board = board;
#endif
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

	board_nodes_mapping.Add(board, new_node, *this);

	return new_node;
}

void MCTS::SimulateWithBoard(GameEngine::Board &board)
{
	while (true) {
		std::vector<GameEngine::Move> next_moves;

		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			board.ApplyMove(GameEngine::Move::GetGameFlowMove(this->GetRandom()));

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
			int choose_move = this->GetRandom() % next_moves.size();
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

void MCTS::BackPropagate(TreeNode *starting_node, bool is_win)
{
	// find all leaf nodes with the same board
	// this simulation can also be seen as simulated from these leaf nodes
	// so we also update them

	GameEngine::Board board;
	starting_node->GetBoard(this->root_node_board, board);

	auto updating_leaf_nodes = this->board_nodes_mapping.Find(board, *this);
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

	TreeNode *most_simulated = nullptr;
	for (const auto &child: node->children) {
		if (most_simulated == nullptr ||
			child->count > most_simulated->count)
		{
			most_simulated = child;
		}
	}

	return most_simulated;
}

void MCTS::PrintBestRoute(int levels)
{
	TreeNode *node = &this->tree.GetRootNode();

	int level = 0;
	while (!node->children.empty() && level <= levels) {

		if (node->move.action != GameEngine::Move::ACTION_GAME_FLOW) {
			PrintLevelPrefix(level);
			std::cout << "[" << node->stage << "] ";
			if (node != &this->tree.GetRootNode()) {
				std::cout << node->move.GetDebugString();
			}
			std::cout << " " << node->wins << "/" << node->count << std::endl;
			level++;
		}

		node = FindBestChildToPlay(node);
		//node = FindBestChildToExpand(node, 0.0);
	}
}

void MCTS::DebugPrint()
{
	int tree_nodes = 0;
	int tree_nodes_size = 0;

	//PrintTree(&this->tree.GetRootNode(), 0, 20);
	PrintBestRoute(10);
}

void MCTS::Iterate()
{
	GameEngine::Board board;

	TreeNode *node = this->SelectAndExpand(board);
	bool is_win = this->Simulate(board);
	this->BackPropagate(node, is_win);
}

void MCTS::BoardNodesMapping::Add(const GameEngine::Board &board, TreeNode *node, const MCTS& mcts)
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);
	this->data[hash].insert(node);
}

std::unordered_set<TreeNode *> MCTS::BoardNodesMapping::Find(const GameEngine::Board &board, const MCTS& mcts)
{
	TreeNode *ret = nullptr;
	std::size_t hash = std::hash<GameEngine::Board>()(board);
	std::unordered_set<TreeNode *> nodes;

	auto possible_nodes = this->data[hash];
	for (const auto &possible_node : possible_nodes) {
		GameEngine::Board it_board;
		possible_node->GetBoard(mcts.root_node_board, it_board);
		if (board == it_board) {
			nodes.insert(possible_node);
		}
	}

	return nodes;
}

void MCTS::BoardNodesMapping::UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map)
{
	for (auto &node : this->data) {
		auto &origin_nodes = node.second;
		std::unordered_set<TreeNode*> new_nodes;
		for (const auto &origin_pointer : origin_nodes) {
			auto it_new_pointer = node_map.find(origin_pointer);
			if (it_new_pointer == node_map.end())
			{
				new_nodes.insert(origin_pointer); // this node is not modified
			}
			else {
				new_nodes.insert(it_new_pointer->second); // this node is modified
			}
		}
		node.second = std::move(new_nodes);
	}
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
	this->board_nodes_mapping = rhs.board_nodes_mapping;
	this->board_nodes_mapping.UpdateNodePointers(node_update_map);
	this->random_generator = rhs.random_generator;

	return *this;
}

MCTS & MCTS::operator=(MCTS&& rhs)
{
	this->root_node_board = std::move(rhs.root_node_board);
	this->tree = std::move(rhs.tree);
	this->board_nodes_mapping = std::move(rhs.board_nodes_mapping);
	this->random_generator = std::move(rhs.random_generator);
	return *this;
}

bool MCTS::operator==(const MCTS& rhs) const
{
	if (this->root_node_board != rhs.root_node_board) return false;
	if (this->tree != rhs.tree) return false;

	// skip checking for internal consistency data
	// if (this->board_nodes_mapping != rhs.board_nodes_mapping) return false;

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

bool MCTS::BoardNodesMapping::operator==(const MCTS::BoardNodesMapping &rhs) const
{
	return this->data == rhs.data;
}

bool MCTS::BoardNodesMapping::operator!=(const MCTS::BoardNodesMapping &rhs) const
{
	return !(*this == rhs);
}

void MCTS::MergeMCTSNodeToParent(const TreeNode *source, TreeNode *parent,
	const std::unordered_map<GameEngine::Move, TreeNode*> &parent_move_map,
	std::unordered_set<GameEngine::Move> &parent_rest_moves)
{
	if (source->move.action == GameEngine::Move::ACTION_GAME_FLOW) {
		if (!parent->moves_not_yet_expanded.empty()) {
			throw std::runtime_error("this should be a game-flow stage, thus should have no non-expanded moves");
		}

		GameEngine::Board board;
		parent->GetBoard(this->root_node_board, board);
		board.ApplyMove(source->move);

		// check if this board has already expanded in parent
		auto it_possible_nodes = this->board_nodes_mapping.Find(board, *this);
		TreeNode *found_node = nullptr;
		for (const auto &it_possible_node : it_possible_nodes) {
			if (it_possible_node->parent == parent) {
				found_node = it_possible_node;
				break;
			}
		}

		if (found_node != nullptr) {
			// merge node data
			this->MergeMCTSNodes(source, found_node);
		}
		else {
			// not found --> this board is not found via the parent node 'node' before
			// --> make a new node
			TreeNode *new_node = new TreeNode;
			TreeNode::CopyWithoutChildren(*source, *new_node);
			parent->AddChild(new_node);
			this->MergeMCTSNodes(source, new_node);
		}
	}
	else {
		auto it_merging_child = parent_move_map.find(source->move);
		if (it_merging_child != parent_move_map.end()) {
			// this move has been traversed in the target
			// --> merge node
			this->MergeMCTSNodes(source, it_merging_child->second);
		}
		else {
			// this move has not been traversed yet
			// make a new node
			TreeNode *new_node = new TreeNode;
			TreeNode::CopyWithoutChildren(*source, *new_node);
			parent->AddChild(new_node);
			parent_rest_moves.erase(source->move);
			this->MergeMCTSNodes(source, new_node);
		}
	}
}

void MCTS::MergeMCTSNodes(const TreeNode *source, TreeNode *target)
{
#ifdef DEBUG
	if (source->stage != target->stage) throw std::runtime_error("consistency check error");
	if (source->stage_type != target->stage_type) throw std::runtime_error("consistency check error");
	if (source->is_player_node != target->is_player_node) throw std::runtime_error("consistency check error");
	if (source->move != target->move) throw std::runtime_error("consistency check error");
#endif

	target->wins += source->wins;
	target->count += source->count;

	if (target->children.empty())
	{
		// move sub-tree directly
		target->moves_not_yet_expanded = std::move(source->moves_not_yet_expanded);
		target->children = std::move(source->children);
		for (const auto &child : target->children) {
			child->parent = target;
		}
		return;
	}

	std::unordered_map<GameEngine::Move, TreeNode*> parent_move_map;
	for (const auto &target_child : target->children) {
		parent_move_map.insert(std::make_pair(target_child->move, target_child));
	}

	auto source_rest_moves = source->moves_not_yet_expanded;
	std::unordered_set<GameEngine::Move> target_rest_moves;

	for (const auto &target_rest_move : target->moves_not_yet_expanded) {
		target_rest_moves.insert(target_rest_move);
	}

	for (const auto &source_child : source->children) {
		this->MergeMCTSNodeToParent(source_child, target, parent_move_map, target_rest_moves);
	}

	target->moves_not_yet_expanded.clear();
	for (const auto &target_rest_move : target_rest_moves) {
		target->moves_not_yet_expanded.push_back(target_rest_move);
	}
}

void MCTS::Merge(const MCTS&& rhs)
{
	if (this->root_node_board != rhs.root_node_board)
	{
		throw std::runtime_error("MCTS with different root boards cannot be merged");
	}

	this->MergeMCTSNodes(&rhs.tree.GetRootNode(), &this->tree.GetRootNode());
}