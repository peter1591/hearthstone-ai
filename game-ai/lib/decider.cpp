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

void Decider::GoToNextProgress(std::vector<ProgressData> &progresses, 
	ProgressData const* stepping_progress, TreeNode const* stepping_node, const GameEngine::Board &next_board)
{
	for (std::vector<ProgressData>::iterator it_progress = progresses.begin(); it_progress != progresses.end();)
	{
		ProgressData &progress = *it_progress;

		// special case: the progress is the stepping progress
		if (&progress == stepping_progress) {
			progress.node = stepping_node;
			if (progress.node->equivalent_node != nullptr) progress.node = progress.node->equivalent_node;
			++it_progress;
		}
		else {
			// find the 'next_board' among the child
			GameEngine::Board current_board = std::move(this->GetCurrentBoard()); // A O(N) algorithm since board cannot be cloned
			progress.node = FindChildNodeWithBoard(progress.node, std::move(current_board), next_board);

			if (progress.node == nullptr) {
				it_progress = progresses.erase(it_progress); // no next node in this MCTS tree
			}
			else {
				++it_progress;
			}
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
bool Decider::GetNextStep(std::vector<ProgressData> &progress, GameEngine::Board &board, MoveInfo &move_info)
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
		// Randomly select a game-flow move
		// Since the MCTS randomly select a game-flow move,
		// this is equivalent to select the most-simulated node
		int r = this->GetRandom() % progress.size();
		ProgressData const& chosen_progress = progress[r];

		TreeNode const* most_simulated_child = FindMostSimulatedChild(chosen_progress.node->children);

		if (most_simulated_child == nullptr) {
			return false;
		}

		move_info.move = most_simulated_child->move;
		move_info.wins = -1;
		move_info.count = -1;

		board.ApplyMove(move_info.move);
		this->GoToNextProgress(progress, &chosen_progress, most_simulated_child, board);
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
		
		board.ApplyMove(move_info.move);
		this->GoToNextProgress(progress, move_info.move);
		return true;
	}
}

GameEngine::Board Decider::GetCurrentBoard()
{
	// TODO: each MCTS should get its current board from its starting board?

	if (this->data.empty()) throw std::runtime_error("logic error");

	GameEngine::Board board = GameEngine::Board::Clone(this->data.front()->current_iteration_root_node_board);

	for (auto const& move : this->best_moves.moves)
	{
		board.ApplyMove(move.move);
	}
	return std::move(board);
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

	GameEngine::Board board = GameEngine::Board::Clone(this->data.front()->current_iteration_root_node_board);

	std::vector<ProgressData> progresses;
	for (int i = 0; i < this->data.size(); ++i) {
		ProgressData progress;
		progress.mcts = this->data[i];
		progress.node = progress.mcts->GetTree().GetRootNode();
		progresses.push_back(progress);
	}

	while (true) {
		MoveInfo move;
		if (this->GetNextStep(progresses, board, move) == false) break;
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