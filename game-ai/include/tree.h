#ifndef GAME_AI_TREE_H
#define GAME_AI_TREE_H

#include <functional>
#include <vector>
#include <unordered_map>
#include <set>
#include "game-engine/board.h"

class TreeNode
{
	public:
		typedef std::list<TreeNode*> children_type;

	public:
		TreeNode() : wins(0), count(0) {}
		TreeNode(const TreeNode &rhs) = delete;
		TreeNode &operator=(const TreeNode &rhs) = delete;
		TreeNode(TreeNode &&rhs);
		TreeNode &operator=(TreeNode &&rhs);

		void AddChild(TreeNode *node);
		void GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const;

		static void CopyWithoutChildren(const TreeNode &source, TreeNode &target);

	public:
		TreeNode *parent;
		children_type children;

		GameEngine::Stage stage;
		GameEngine::StageType stage_type;
#ifdef DEBUG_SAVE_BOARD
		GameEngine::Board board;
#endif

		// what move lead us from parent to this state?
		GameEngine::Move move;

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
		std::vector<GameEngine::Move> moves_not_yet_expanded;
};

class Tree
{
	public:
		Tree();
		~Tree();

		Tree(const Tree& rhs) = delete;
		Tree & operator=(const Tree& rhs) = delete;

		Tree(Tree&& rhs);
		Tree & operator=(Tree&& rhs);

		Tree Clone(std::function<void(TreeNode*, TreeNode*)> node_update_callback) const;
		bool operator==(const Tree& rhs) const;
		bool operator!=(const Tree& rhs) const;

	public:
		TreeNode & GetRootNode() { return this->root_node; }
		const TreeNode & GetRootNode() const { return this->root_node; }

	private:
		static void ClearSubtree(TreeNode *node);
		static void CopySubtree(const TreeNode *source, TreeNode *target, std::function<void(TreeNode*, TreeNode*)> node_update_callback);
		static bool CompareSubtree(const TreeNode *lhs, const TreeNode *rhs);

	private:
		TreeNode root_node;
};

inline TreeNode::TreeNode(TreeNode &&rhs)
{
	*this = std::move(rhs);
}

inline TreeNode & TreeNode::operator=(TreeNode &&rhs)
{
	this->parent = rhs.parent;
	this->children = std::move(rhs.children);
	this->stage = rhs.stage;
	this->stage_type = rhs.stage_type;
#ifdef DEBUG_SAVE_BOARD
	this->board = std::move(rhs.board);
#endif
	this->move = std::move(rhs.move);
	this->is_player_node = std::move(rhs.is_player_node);
	this->wins = rhs.wins;
	this->count = rhs.count;
	this->moves_not_yet_expanded = std::move(rhs.moves_not_yet_expanded);
	return *this;
}

inline void TreeNode::AddChild(TreeNode *node)
{
	this->children.push_back(node);
	node->parent = this;
}

inline void TreeNode::GetBoard(const GameEngine::Board &root_node_board, GameEngine::Board &board) const
{
	if (this->parent == nullptr) {
		board = root_node_board;
		return;
	}

	this->parent->GetBoard(root_node_board, board);
	board.ApplyMove(this->move);

#ifdef DEBUG_SAVE_BOARD
	if (board != this->board) {
		throw std::runtime_error("node board not match");
	}
#endif
}

inline Tree::Tree()
{
}

inline Tree::~Tree()
{
	Tree::ClearSubtree(&this->root_node);
}

inline Tree::Tree(Tree&& rhs)
{
	*this = std::move(rhs);
}

inline Tree& Tree::operator=(Tree&& rhs)
{
	this->ClearSubtree(&this->root_node);
	for (auto &first_level_child : rhs.root_node.children)
	{
		first_level_child->parent = &this->root_node;
	}
	this->root_node = std::move(rhs.root_node);

	return *this;
}

inline void Tree::ClearSubtree(TreeNode *node)
{
	for (const auto &child : node->children)
	{
		Tree::ClearSubtree(child);
		delete child;
	}
	node->children.clear();
}

inline void TreeNode::CopyWithoutChildren(const TreeNode &source, TreeNode &target)
{
	target.parent = nullptr;
	target.stage = source.stage;
	target.stage_type = source.stage_type;
#ifdef DEBUG_SAVE_BOARD
	target.board = source.board;
#endif
	target.move = source.move;
	target.is_player_node = source.is_player_node;
	target.wins = source.wins;
	target.count = source.count;
	target.moves_not_yet_expanded = source.moves_not_yet_expanded;
}

inline void Tree::CopySubtree(const TreeNode *source, TreeNode *target, std::function<void(TreeNode*, TreeNode*)> node_update_callback)
{
#ifdef DEBUG
	if (!target->children.empty()) {
		throw std::runtime_error("should clear target first");
	}
#endif

	for (const auto &child : source->children)
	{
		TreeNode *new_node = new TreeNode;
		TreeNode::CopyWithoutChildren(*child, *new_node);
		target->AddChild(new_node);
		node_update_callback(child, new_node);

		Tree::CopySubtree(child, new_node, node_update_callback);
	}
}

inline Tree Tree::Clone(std::function<void(TreeNode*, TreeNode*)> node_update_callback) const
{
	Tree new_tree;

	new_tree.root_node.parent = nullptr;
	new_tree.root_node.children.clear();
	TreeNode::CopyWithoutChildren(this->root_node, new_tree.root_node);

	// copy child nodes
	Tree::CopySubtree(&this->root_node, &new_tree.root_node, node_update_callback);

	return std::move(new_tree);
}

inline bool Tree::CompareSubtree(const TreeNode *lhs, const TreeNode *rhs)
{
	if (lhs->stage != rhs->stage) return false;
	if (lhs->stage_type != rhs->stage_type) return false;
	if (lhs->move != rhs->move) return false;
	if (lhs->is_player_node != rhs->is_player_node) return false;
	if (lhs->wins != rhs->wins) return false;
	if (lhs->count != rhs->count) return false;
	if (lhs->moves_not_yet_expanded != rhs->moves_not_yet_expanded) return false;

	if (lhs->children.size() != rhs->children.size()) return false;

	auto lhs_child = lhs->children.begin();
	auto rhs_child = rhs->children.begin();
	while (lhs_child != lhs->children.end() && rhs_child != rhs->children.end()) {
		if (Tree::CompareSubtree(*lhs_child, *rhs_child) == false) return false;
		lhs_child++;
		rhs_child++;
	}

	return true;
}

inline bool Tree::operator==(const Tree& rhs) const
{
	return Tree::CompareSubtree(&this->root_node, &rhs.root_node);
}

inline bool Tree::operator!=(const Tree& rhs) const
{
	return !(*this == rhs);
}

#endif
