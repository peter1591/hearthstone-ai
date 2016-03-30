#include "game-ai/decider.h"

static void PrintLevelPrefix(int level)
{
	for (int i = 0; i<level; i++) {
		std::cout << "..";
	}
}

static TreeNode *FindBestChildToPlay(const TreeNode *node)
{
	if (node->children.empty()) throw std::runtime_error("failed");

	TreeNode *most_simulated = nullptr;
	for (const auto &child : node->children) {
		if (most_simulated == nullptr ||
			child->count > most_simulated->count)
		{
			most_simulated = child;
		}
	}

	return most_simulated;
}

void Decider::PrintBestRoute(int levels)
{
	const MCTS& mcts = *this->data.front();

	Tree const& tree = mcts.GetTree();

	TreeNode const* node = tree.GetRootNode();

	int level = 0;
	while (node != nullptr && !node->children.empty() && level <= levels) {
		if (node->equivalent_node != nullptr) node = node->equivalent_node;

		if (node->move.action != GameEngine::Move::ACTION_GAME_FLOW) {
			PrintLevelPrefix(level);
			std::cout << "[" << node->stage << "] ";
			if (node != tree.GetRootNode()) {
				std::cout << node->move.GetDebugString();
			}
			std::cout << " " << node->wins << "/" << node->count << std::endl;
			level++;
		}

		node = FindBestChildToPlay(node);
	}
}

void Decider::PrintTree(TreeNode const* node, int level, const int max_level)
{
	const MCTS& mcts = *this->data.front();

	if (level >= max_level) return;

	PrintLevelPrefix(level);
	std::cout << "[" << node->stage << "] ";
	if (node != mcts.GetTree().GetRootNode()) {
		std::cout << node->move.GetDebugString();
	}
	std::cout << " " << node->wins << "/" << node->count << std::endl;

	if (node->equivalent_node != nullptr) node = node->equivalent_node;

	for (auto const& child : node->children) {
		this->PrintTree(child, level + 1, max_level);
	}
}

static TreeNode const* FindMostSimulatedChild(std::list<TreeNode*> const& children)
{
	if (children.empty()) return nullptr;

	TreeNode const* most_simulated_node = children.front();
	int most_simulated_count = most_simulated_node->count;
	for (auto const& child : children)
	{
		if (child->count > most_simulated_count) {
			most_simulated_node = child;
			most_simulated_count = most_simulated_node->count;
		}
	}
	return most_simulated_node;
}

static std::unordered_map<GameEngine::Move, TreeNode>::const_iterator FindMostSimulatedChild(std::unordered_map<GameEngine::Move, TreeNode> const& children)
{
	if (children.empty()) return children.end();

	auto it_most_simulated_node = children.begin();
	int most_simulated_count = it_most_simulated_node->second.count;
	for (auto it_child = children.begin(); it_child != children.end(); ++it_child)
	{
		if (it_child->second.count > most_simulated_count) {
			it_most_simulated_node = it_child;
			most_simulated_count = it_most_simulated_node->second.count;
		}
	}
	return it_most_simulated_node;
}

static TreeNode const* FindChildNodeWithBoard(TreeNode const* parent, GameEngine::Board && parent_board, GameEngine::Board const& child_board)
{
	for (auto child : parent->children)
	{
		if (child->equivalent_node != nullptr) child = child->equivalent_node;

		GameEngine::Board current_child_board = std::move(parent_board);
		current_child_board.ApplyMove(child->move);
		if (current_child_board == child_board) return child;
	}

	return nullptr;
}

void Decider::GoToNextDeterministicGameFlowProgress(std::vector<ProgressData> &progresses)
{
	for (std::vector<ProgressData>::iterator it_progress = progresses.begin(); it_progress != progresses.end();)
	{
		ProgressData &progress = *it_progress;

		if (progress.node->children.empty()) {
			it_progress = progresses.erase(it_progress); // no next node in this MCTS tree
		}
		else if (progress.node->children.size() == 1) {
			progress.node = progress.node->children.front();
			++it_progress;
		}
		else {
			throw std::runtime_error("this node is not a deterministic game-flow move");
		}
	}
}

void Decider::GoToNextProgress(std::vector<ProgressData> &progresses, GameEngine::Move const& move)
{
	for (std::vector<ProgressData>::iterator it_progress = progresses.begin(); it_progress != progresses.end();)
	{
		ProgressData &progress = *it_progress;

		bool found = false;
		for (auto const& child : progress.node->children)
		{
			if (child->move == move) {
				progress.node = child;
				if (progress.node->equivalent_node != nullptr) progress.node = progress.node->equivalent_node;
				found = true;
				break;
			}
		}

		if (found) {
			++it_progress;
		}
		else {
			it_progress = progresses.erase(it_progress); // no next node in this MCTS tree
		}
	}
}

std::unordered_map<GameEngine::Move, TreeNode> Decider::AggregateProgressChildren(std::vector<ProgressData> const& progresses)
{
	std::unordered_map<GameEngine::Move, TreeNode> result;

	for (auto const& progress : progresses)
	{
		for (auto const& child : progress.node->children)
		{
			auto it_result = result.find(child->move);
			if (it_result == result.end()) {
				TreeNode new_result;
				new_result.wins = child->wins;
				new_result.count = child->count;
				result.insert(it_result, std::make_pair(child->move, new_result));
			}
			else {
				// aggregate
				it_result->second.wins += child->wins;
				it_result->second.count += child->count;
			}
		}
	}
	return result;
}

// return false if no next step is available
bool Decider::GetNextStep(std::vector<ProgressData> &progress, MoveInfo &move_info)
{
	if (progress.empty()) return false;

#ifdef DEBUG
	for (int i = 1; i < progress.size(); i++) {
		if (progress[i].node->stage != progress[0].node->stage) throw std::runtime_error("consistency check failed");
		if (progress[i].node->stage_type != progress[0].node->stage_type) throw std::runtime_error("consistency check failed");
	}
#endif

	TreeNode const* node_ref = progress.front().node;

	if (node_ref->stage_type == GameEngine::STAGE_TYPE_GAME_END) {
		return false;
	}
	else if (node_ref->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
		// If there's only one game-flow outcome (i.e., deterministic)
		// Then, the decider can get the next move
		// However, if there's several different outcomes (i.e., non-deterministic)
		// Then, the best move depends on the actual outcome after play
		// It's meaningless if we choose one of the non-deterministic outcomes here

		for (auto const& current_progress : progress) {
			if (current_progress.node->children.size() > 1) {
				return false;
			}
		}

		this->GoToNextDeterministicGameFlowProgress(progress);

		move_info.move.action = GameEngine::Move::ACTION_GAME_FLOW;
		move_info.move.data.game_flow_data.rand_seed = 0;
		move_info.wins = -1;
		move_info.count = -1;

		return true;
	}
	else {
		// select the best move --> the most simulated node
		std::unordered_map<GameEngine::Move, TreeNode> aggregated_children = this->AggregateProgressChildren(progress);
		
		auto it_most_simulated_child = FindMostSimulatedChild(aggregated_children);
		if (it_most_simulated_child == aggregated_children.end()) {
			return false;
		}

		move_info.move = it_most_simulated_child->first;
		move_info.wins = it_most_simulated_child->second.wins;
		move_info.count = it_most_simulated_child->second.count;
		
		this->GoToNextProgress(progress, move_info.move);
		return true;
	}
}

void Decider::DebugPrint()
{
	//this->PrintTree(&this->mcts.tree.GetRootNode(), 0, 5);
	this->PrintBestRoute(20);
}

void Decider::Add(const MCTS& mcts)
{
	this->data.push_back(&mcts);
}

int Decider::GetRandom()
{
	return rand();
}

Decider::MovesInfo Decider::GetBestMoves()
{
	this->best_moves.Clear();

	if (this->data.empty()) {
		throw std::runtime_error("no any MCTS available");
	}

	// check root node boards are identical
	for (int i = 1; i < this->data.size(); ++i) {
		MCTS const& mcts_ref = *this->data.front();
		MCTS const& mcts_current = *this->data[i];
	}

	std::vector<ProgressData> progresses;
	for (int i = 0; i < this->data.size(); ++i) {
		ProgressData progress;
		progress.mcts = this->data[i];
		progress.node = progress.mcts->GetTree().GetRootNode();
		progresses.push_back(progress);
	}

	while (true) {
		MoveInfo move;
		if (this->GetNextStep(progresses, move) == false) break;
		this->best_moves.moves.push_back(move);
	}

	return this->best_moves;
}

void Decider::MovesInfo::DebugPrint()
{
	for (auto const& move : this->moves)
	{
		std::cout << "Got move: " << move.move.GetDebugString() << ", " << move.wins << "/" << move.count << std::endl;
	}
}