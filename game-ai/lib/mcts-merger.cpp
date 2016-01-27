#include "mcts-merger.h"

MCTSMerger::MCTSMerger(MCTS &mcts) : mcts(mcts)
{

}

MCTS& MCTSMerger::Get()
{
	return this->mcts;
}

void MCTSMerger::Merge(MCTS&& rhs)
{
	if (this->mcts.root_node_board != rhs.root_node_board)
	{
		throw std::runtime_error("MCTS with different root boards cannot be merged");
	}

	this->Merge(&rhs.tree.GetRootNode(), &this->mcts.tree.GetRootNode(), this->mcts.root_node_board, std::move(rhs));
}

void MCTSMerger::Merge(TreeNode *source, TreeNode *target, const GameEngine::Board &board, MCTS &&source_mcts)
{
#ifdef DEBUG
	if (source->stage != target->stage) throw std::runtime_error("consistency check error");
	if (source->stage_type != target->stage_type) throw std::runtime_error("consistency check error");
	if (source->is_player_node != target->is_player_node) throw std::runtime_error("consistency check error");
#endif

	target->wins += source->wins;
	target->count += source->count;

	if (target->children.empty())
	{
		return this->MergeToTargetWithNoChildren(source, target, board, std::move(source_mcts));
	}

	auto source_rest_moves = source->moves_not_yet_expanded;
	std::unordered_set<GameEngine::Move> target_rest_moves;

	for (const auto &target_rest_move : target->moves_not_yet_expanded) {
		target_rest_moves.insert(target_rest_move);
	}

	for (const auto &source_child : source->children) {
		this->MergeToParent(source_child, target, board, target_rest_moves, std::move(source_mcts));
	}

	target->moves_not_yet_expanded.clear();
	for (const auto &target_rest_move : target_rest_moves) {
		target->moves_not_yet_expanded.push_back(target_rest_move);
	}
}

void MCTSMerger::MergeToParent(TreeNode *source, TreeNode *parent, const GameEngine::Board &parent_board,
	std::unordered_set<GameEngine::Move> &parent_rest_moves, MCTS&& source_mcts)
{
	GameEngine::Board board = parent_board;
	board.ApplyMove(source->move);

#ifdef CHECK_MOVE_REAPPLIABLE
	if (source->board != board) throw std::runtime_error("consistency check failed");
#endif

	// check if this board has already expanded in parent
	auto it_possible_nodes = this->mcts.board_node_map.Find(board, this->mcts);
	TreeNode *target_node = nullptr;
	for (const auto &it_possible_node : it_possible_nodes) {
		if (it_possible_node->parent == parent) {
			target_node = it_possible_node;
			break;
		}
	}

	if (target_node != nullptr) {
		// merge node data
		parent_rest_moves.erase(target_node->move);
	}
	else {
		// not found --> this board is not found via the parent node 'node' before
		// --> make a new node
		target_node = new TreeNode;
		TreeNode::CopyWithoutChildren(*source, *target_node);
		parent->AddChild(target_node);
		this->mcts.board_node_map.Add(board, target_node, this->mcts);
	}

	this->Merge(source, target_node, board, std::move(source_mcts));
}

void MCTSMerger::MergeToTargetWithNoChildren(TreeNode *source, TreeNode *target, const GameEngine::Board &board, MCTS &&source_mcts)
{
	if (source->children.empty()) return; // nothing to do
#ifdef DEBUG
	if (target->children.empty() == false) {
		throw std::runtime_error("consistency check failed");
	}
#endif

	// the hash mapping of the new nodes and their boards
	// the new nodes are the tree nodes moved to the 'target'
	std::map<TreeNode *, std::size_t> new_nodes_hash_map;

	// Step 1. move the subtree to target
	target->children.swap(source->children);
	for (const auto &child : target->children) {
		child->parent = target;
	}

	// Step 2. fill the tree nodes' pointers only
	std::queue<TreeNode*> waiting;

	// push the target's children first, since we don't want push 'target' to 'new_nodes_hash_map'
	for (const auto &child : target->children) {
		waiting.push(child);
	}

	while (!waiting.empty()) {
		TreeNode *processing = waiting.front();
		waiting.pop();

		new_nodes_hash_map[processing] = std::size_t(); // fill the hash later

		for (const auto &child : processing->children) {
			waiting.push(child);
		}
	}

	// Step 3. scan the MCTS's hash mapping to fill the hashs
	source_mcts.board_node_map.FillHash(new_nodes_hash_map);

	// Step 4. erase the mapping from source
	source_mcts.board_node_map.Erase(new_nodes_hash_map);

	// Step 5. add the new mapping to target
	for (const auto &new_mapping : new_nodes_hash_map)
	{
		TreeNode* const& tree_node = new_mapping.first;
		std::size_t const& hash = new_mapping.second;
		this->mcts.board_node_map.Add(hash, tree_node);
	}
}