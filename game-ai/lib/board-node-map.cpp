#include "mcts.h"
#include "board-node-map.h"

bool BoardNodeMap::operator==(const BoardNodeMap &rhs) const
{
	return this->data == rhs.data;
}

bool BoardNodeMap::operator!=(const BoardNodeMap &rhs) const
{
	return !(*this == rhs);
}

void BoardNodeMap::Add(const GameEngine::Board &board, TreeNode *node, const MCTS& mcts)
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);
	this->data[hash].insert(node);
}

void BoardNodeMap::Add(std::size_t board_hash, TreeNode *node)
{
	this->data[board_hash].insert(node);
}

std::unordered_set<TreeNode *> BoardNodeMap::Find(const GameEngine::Board &board, const MCTS& mcts)
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

void BoardNodeMap::UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map)
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

void BoardNodeMap::FillHash(std::map<TreeNode*, std::size_t> &nodes) const
{
	for (const auto &node : this->data) {
		const std::size_t &hash = node.first;
		for (const auto &tree_node : node.second) {
			auto it_filling_node = nodes.find(tree_node);

			if (it_filling_node == nodes.end()) continue;
			it_filling_node->second = hash;
		}
	}
}

void BoardNodeMap::Clear()
{
	this->data.clear();
}

void BoardNodeMap::Erase(const std::map<TreeNode *, std::size_t> &erasing_nodes)
{
	for (const auto &node : erasing_nodes) {
		TreeNode* const& tree_node = node.first;
		std::size_t const& hash = node.second;

		auto existing_nodes = this->data[hash];
		existing_nodes.erase(tree_node);
	}
}