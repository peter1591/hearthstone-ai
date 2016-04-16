#ifndef GAME_AI_TREE_H
#define GAME_AI_TREE_H

#include <queue>
#include <functional>
#include <vector>
#include <unordered_map>
#include <set>
#include "game-engine/board.h"
#include "traversed-path-recorder.h"

class TreeNode
{
public:
	typedef std::list<TreeNode*> children_type;

public:
	TreeNode() : parent(nullptr), equivalent_node(nullptr) {}

	void AddChild(TreeNode *node);
	void GetBoard(GameEngine::Board & initialized_board) const;
	TreeNode * FindChildByMove(GameEngine::Move const& move);

public:
	TreeNode *parent;
	children_type children;

	TreeNode *equivalent_node; // the board is the same as this node, only move/count are valid info for this node

public:
	GameEngine::Stage stage;
	GameEngine::StageType stage_type;

	std::unique_ptr<std::unordered_map<GameEngine::Move, TreeNode *>> move_child_map;

	// what move lead us from parent to this state?
	GameEngine::Move move;
	int start_board_random; // (any) one of the start board randoms which leads us to this node
	TraversedPathRecorder path; // one of the paths leading to this node

	int turn;

	// if this is a player's turn, then it's true
	// if this is a random node, and it came out due to a player's action, then it's true
	// otherwise, it's false
	bool is_player_node;

	// if it's player's turn, this is the number of WINNINGS in all children
	// if it's opponent's turn, this is the number of LOSSES in all children
	int wins;

	// number of simulations
	int count;

	// only valid if this node is expanded (i.e., has children)
	std::unique_ptr<GameEngine::NextMoveGetter> next_move_getter; // not valid if next moves are not deterministic
	bool next_moves_are_deterministic; // The next moves always the same for identical boards? E.g., Game-flow are not deterministic

	// for a non-deterministic move, the select probability of its children are not uniform
	// this number tracks the total times this child node have been chosen
	int nondeterminstic_move_selected_times;

#ifdef DEBUG
	size_t board_hash;
#endif
};

class Tree
{
public:
	Tree();
	~Tree();

	Tree(const Tree& rhs) = delete;
	Tree & operator=(const Tree& rhs) = delete;

	Tree(Tree&& rhs) = delete;
	Tree & operator=(Tree&& rhs) = delete;

public:
	void CreateRootNode();
	TreeNode * GetRootNode();
	TreeNode const* GetRootNode() const;

	void Clear();

private:
	static void ClearSubtree(TreeNode *node);

private:
	TreeNode * root_node;

};

inline void TreeNode::AddChild(TreeNode *node)
{
	this->children.push_back(node);
	node->parent = this;

	if (this->move_child_map) {
		this->move_child_map->insert(std::make_pair(node->move, node));
	}
}

inline void TreeNode::GetBoard(GameEngine::Board & board) const
{
	std::list<TreeNode const*> path_nodes;
	auto path_iterator = this->path.GetReverseIterator(this);

	while (true) {
		auto const& node = path_iterator.GetNodeAndMoveUpward();
		if (node == nullptr) break;
		if (node->parent == nullptr) break;
		path_nodes.push_front(node);
	}

	for (auto const& path_node : path_nodes) {
		board.ApplyMove(path_node->move);
	}

#ifdef DEBUG
	size_t current_hash = std::hash<GameEngine::Board>()(board);
	if (this->board_hash != current_hash) {
		throw std::runtime_error("consistency check failed: board hash changed");
	}
#endif
}

inline TreeNode * TreeNode::FindChildByMove(GameEngine::Move const & child_move)
{
	if (this->move_child_map) {
		auto it = this->move_child_map->find(child_move);
		if (it == this->move_child_map->end()) return nullptr;
		return it->second;
	}

	// create the map on demand
	this->move_child_map.reset(new std::unordered_map<GameEngine::Move, TreeNode*>());

	TreeNode * ret = nullptr;
	for (auto const& child : this->children) {
		this->move_child_map->insert(std::make_pair(child->move, child));
		if (child->move == child_move) ret = child;
	}

	return ret;
}

inline Tree::Tree()
{
	this->root_node = nullptr;
}

inline Tree::~Tree()
{
	if (this->root_node) {
		Tree::ClearSubtree(this->root_node);
		this->root_node = nullptr;
	}
}

inline void Tree::CreateRootNode()
{
	if (this->root_node) throw std::runtime_error("root node already exists");
	this->root_node = new TreeNode;
}

inline TreeNode * Tree::GetRootNode()
{
	return this->root_node;
}

inline TreeNode const* Tree::GetRootNode() const
{
	return this->root_node;
}

inline void Tree::Clear()
{
	if (this->root_node) {
		this->ClearSubtree(this->root_node);
		this->root_node = nullptr;
	}
}

inline void Tree::ClearSubtree(TreeNode *node)
{
	for (const auto &child : node->children)
	{
		Tree::ClearSubtree(child);
	}
	delete node;
}

#endif
