#include "mcts.h"
#include "board-node-map.h"

TreeNode * BoardNodeMap::Find(const GameEngine::Board &board, const MCTS& mcts) const
{
	std::size_t hash = std::hash<GameEngine::Board>()(board);

	auto it_found = this->map.find(hash);
	if (it_found == this->map.end()) return nullptr;

	for (const auto &possible_node : it_found->second) {
		GameEngine::Board it_board;
		possible_node->GetBoard(mcts.root_node_board, it_board);
		if (board == it_board) return possible_node;
	}

	return nullptr;
}

static void UpdateNodePointersInternal(
	std::unordered_map<std::size_t, std::unordered_set<TreeNode*> > &data,
	std::unordered_map<TreeNode*, TreeNode*> const& node_map)
{
	for (auto &node : data) {
		std::unordered_set<TreeNode*> new_nodes;

		for (const auto &origin_pointer : node.second) {
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

void BoardNodeMap::UpdateNodePointers(const std::unordered_map<TreeNode*, TreeNode*>& node_map)
{
	UpdateNodePointersInternal(this->map, node_map);
}

void BoardNodeMap::FillHash(std::map<TreeNode*, std::size_t> &nodes) const
{
	for (const auto &node : this->map) {
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
	this->map.clear();
}