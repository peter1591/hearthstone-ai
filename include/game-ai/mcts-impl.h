#pragma once

#include "mcts.h"
#include "turn.h"

inline MCTS::MCTS()
{
	this->allocated_node = new TreeNode;
}

inline MCTS::~MCTS()
{
	if (this->allocated_node) delete this->allocated_node;
}

inline void MCTS::Initialize(unsigned int rand_seed, std::unique_ptr<BoardInitializer> && board_initializer)
{
	srand(rand_seed);
	this->board_initializer = std::move(board_initializer);
}

inline void MCTS::Iterate()
{
	this->current_start_board_random = rand();

#ifdef DEBUG_PRINT_MOVE
	std::cout << "MCTS iterate start with random: " << this->current_start_board_random << std::endl;
#endif

	GameEngine::Board board;
	this->board_initializer->InitializeBoard(this->current_start_board_random, board);
	this->traversed_path.Clear();

	if (!this->tree.GetRootNode()) {
		this->CreateRootNode(board);
	}
	TreeNode *node = this->tree.GetRootNode();

#ifdef DEBUG
	if (!this->traversed_nodes.empty()) throw std::runtime_error("consistency check failed");
#endif

	this->traversed_nodes.push_back(node);

	this->SelectAndExpand(node, board);
	bool is_win = this->Simulate(board);
	this->BackPropagate(is_win);
}

inline Tree const & MCTS::GetTree() const
{
	return this->tree;
}

inline void MCTS::CreateRootNode(GameEngine::Board const& board)
{
	tree.CreateRootNode();
	TreeNode * root_node = tree.GetRootNode();

	root_node->start_board_random = this->current_start_board_random;
	root_node->path.Clear();
	root_node->stage = board.GetStage();
	root_node->stage_type = board.GetStageType();
	root_node->parent = nullptr;
	root_node->turn = Turn::GetTurnForRootNode();
	root_node->wins = 0;
	root_node->count = 0;
	if (root_node->stage_type == GameEngine::STAGE_TYPE_PLAYER) {
		root_node->is_player_node = true;
	}
	else {
		// Note: if the starting node is a RANDOM node,
		// then the root node's is_player_node doesn't matter
		root_node->is_player_node = false;
	}
#ifdef DEBUG
	root_node->board_hash = std::hash<GameEngine::Board>()(board);
#endif

	this->board_finder.Add(board, root_node);
}

inline void MCTS::SelectAndExpand(TreeNode* & node, GameEngine::Board & board)
{
	while (true)
	{
#ifdef DEBUG
		if (node->equivalent_node != nullptr) throw std::runtime_error("consistency check failed");
		size_t current_hash = std::hash<GameEngine::Board>()(board);
		if (node->board_hash != current_hash) throw std::runtime_error("consistency check failed: board hash changed");
#endif

		if (node->stage_type == GameEngine::STAGE_TYPE_GAME_END) return;

		if (node->children.empty()) {
			// not expanded before
			if (this->ExpandNewNode(node, board)) return;
		}
		else if (node->next_moves_are_deterministic) {
			if (this->ExpandNodeWithDeterministicNextMoves(node, board)) return;
		}
		else {
			if (!this->UseNextMoveGetter(node)) {
				// quick process if we have only one next move
				if (this->ExpandNodeWithSingleRandomNextMove(node, board)) return;
			}
			else {
				if (this->ExpandNodeWithMultipleRandomNextMoves(node, board)) return;
			}
		}
	}
}

inline bool MCTS::Simulate(GameEngine::Board &board)
{
	while (true) {
		GameEngine::StageType stage_type = board.GetStageType();

		if (stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) {
			GameEngine::Move next_move;
			bool is_deterministic;
			board.GetNextMoves(next_move, is_deterministic);
			board.ApplyMove(next_move);

		}
		else if (stage_type == GameEngine::STAGE_TYPE_GAME_END) {
			return (board.GetStage() == GameEngine::STAGE_WIN);

		}
		else {
			GameEngine::Move move;
			board.GetGoodMove(move, this->GetRandom());
			board.ApplyMove(move);
		}
	}
}

inline void MCTS::BackPropagate(bool is_win)
{
	while (!this->traversed_nodes.empty())
	{
		TreeNode *node = this->traversed_nodes.back();
		this->traversed_nodes.pop_back();

		if (node->equivalent_node != nullptr) {
			node->count++; // only update simulation count for redirect nodes
			node = node->equivalent_node;
		}

		if (node->is_player_node) {
			if (is_win == true) node->wins++; // from the player's respect
		}
		else {
			if (is_win == false) node->wins++; // from the opponent's respect
		}
		node->count++;
	}
}

inline bool MCTS::UseNextMoveGetter(TreeNode * node)
{
	if (node->stage_type == GameEngine::STAGE_TYPE_GAME_FLOW) return false;
	else return true;
}

inline int MCTS::GetRandom()
{
	return rand();
}

// return true if a new (normal) node is created; 'new_node' will be the created new child
// return false is a redirect node is created; 'new_node' will be the target node
inline TreeNode * MCTS::CreateChildNode(TreeNode* const node, GameEngine::Move const& next_move, GameEngine::Board & next_board)
{
	// Note: now the 'next_board' is the node 'node' plus the move 'next_move'

	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->start_board_random = this->current_start_board_random;
	new_node->path = this->traversed_path; // copy
	new_node->equivalent_node = nullptr;
#ifdef DEBUG
	if (&new_node->move != &next_move) throw std::runtime_error("we've optimized by assuming next_move === this->allocated_node->move");
	//new_node->move = expanding_move; // optimized out
#endif
	new_node->turn = Turn::GetTurn(node, next_move);
	new_node->wins = 0;
	new_node->count = 0;
	new_node->stage = next_board.GetStage();
	new_node->stage_type = next_board.GetStageType();

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

#ifdef DEBUG
	new_node->board_hash = std::hash<GameEngine::Board>()(next_board);

	//GameEngine::Board test_board = this->start_board.GetBoard(new_node->start_board_random);
	//new_node->GetBoard(test_board);
	//if (next_board != test_board) throw std::runtime_error("cannot deduce board repeatedly");
#endif

	this->board_finder.Add(next_board, new_node);
	this->traversed_nodes.push_back(new_node);

	return new_node;
}

inline TreeNode * MCTS::CreateRedirectNode(TreeNode * parent, GameEngine::Move const& move, TreeNode * target_node)
{
	TreeNode * new_node = this->allocated_node;
	this->allocated_node = new TreeNode;

	new_node->move = move;
	new_node->wins = 0;
	new_node->count = 0;
	new_node->equivalent_node = target_node;
	new_node->start_board_random = this->current_start_board_random; // it's not used, but it's harmless to fill it anyway
	parent->AddChild(new_node);

	return new_node;
}

inline TreeNode * MCTS::FindBestChildToExpand(std::list<TreeNode*> const & children)
{
	if (children.empty()) return nullptr;

	int total_simulations = 0;
	for (auto const& child : children) {
		total_simulations += child->count;
	}

	TreeNode::children_type::const_iterator it_child = children.begin();
	double total_simulations_ln = log((double)total_simulations);

	TreeNode * max_weight_node = *it_child;
	double max_weight = this->CalculateSelectionWeight(*it_child, total_simulations_ln);
	++it_child;

	for (; it_child != children.end(); ++it_child) {
		double weight = this->CalculateSelectionWeight(*it_child, total_simulations_ln);
		if (weight > max_weight) {
			max_weight = weight;
			max_weight_node = *it_child;
		}
	}

	return max_weight_node;
}

inline double MCTS::CalculateSelectionWeight(TreeNode * node, double total_simulations_ln)
{
	auto const& node_win = node->wins;
	auto const& node_simulations = node->count;

	double win_rate;
	if (node->equivalent_node == nullptr) {
		// normal node
		win_rate = (double)node_win / node_simulations;
	}
	else {
		// redirect node
		win_rate = (double)node->equivalent_node->wins / node->equivalent_node->count;
	}

	double exploration_term = sqrt(total_simulations_ln / node_simulations);

	double variance = win_rate * (1.0 - win_rate);
	double ucb1_tuned_factor = variance + sqrt(2.0 * total_simulations_ln / node_simulations);
	if (ucb1_tuned_factor < 0.25) ucb1_tuned_factor = 0.25;

	return win_rate + exploration_term * ucb1_tuned_factor;
}

inline bool MCTS::ExpandNewNode(TreeNode * & node, GameEngine::Board & board)
{
	GameEngine::Move & expanding_move = this->allocated_node->move;

	if (this->UseNextMoveGetter(node)) {
		node->next_move_getter.reset(new GameEngine::NextMoveGetter());
		board.GetNextMoves(*node->next_move_getter, node->next_moves_are_deterministic);
		if (node->next_move_getter->GetNextMove(board, expanding_move) == false) {
			throw std::runtime_error("should at least return one possible move");
		}
		if (!node->next_moves_are_deterministic) {
			// we will not use next move getter for non-deterministic nodes
			node->next_move_getter.reset(nullptr);
		}
	}
	else {
		board.GetNextMoves(expanding_move, node->next_moves_are_deterministic);
	}

	board.ApplyMove(expanding_move);

	// find transposition node (i.e., other node with the same board)
	TreeNode *transposition_node = this->board_finder.Find(Turn::GetTurn(node, expanding_move), board, this->board_initializer.get());
	if (transposition_node) {
		// a transposition node is found
#ifdef DEBUG
		if (transposition_node->parent == node) throw std::runtime_error("logic error: 'node' should have no parent in ExpandNewNode()");
		if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");
#endif

		TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, transposition_node);
		this->traversed_nodes.push_back(redirect_node);
		this->traversed_path.AddRedirectNode(redirect_node);

		node = transposition_node;
		return false;
	}

	node = this->CreateChildNode(node, expanding_move, board);
	return true;
}

inline bool MCTS::ExpandNodeWithDeterministicNextMoves(TreeNode * & node, GameEngine::Board & board)
{
	// next moves are deterministic, and they are stored in next_move_getter

	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	if (!this->UseNextMoveGetter(node)) throw std::runtime_error("game-flow stages should all with non-deterministic next moves");
#endif

	if (node->next_move_getter) {
		if (node->next_move_getter->GetNextMove(board, expanding_move)) {
			// not fully expanded yet
#ifdef DEBUG
			for (auto const& child : node->children) {
				if (child->move == expanding_move) throw std::runtime_error("next-move-getter returns one particular move twice!");
			}
#endif

			board.ApplyMove(expanding_move);

			TreeNode *transposition_node = this->board_finder.Find(Turn::GetTurn(node, expanding_move), board, this->board_initializer.get());
			if (transposition_node) {
#ifdef DEBUG
				if (transposition_node->parent == node) {
					// expanded before under the same parent but with different move
					// --> no need to create a new redirect node
					throw std::runtime_error("two different moves under the same parent yield identical game states, why is this happening?");
					// Note: we can still create redirect node, but we want to know why this is happening
				}
				if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");

				for (auto const& child_node : node->children) {
					if (child_node->equivalent_node == transposition_node) {
						throw std::runtime_error("two different moves yield an identical board, why is this happening?");
					}
				}
#endif

				// create a redirect node
				TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, transposition_node);
				this->traversed_nodes.push_back(redirect_node);
				this->traversed_path.AddRedirectNode(redirect_node);

				node = transposition_node;
				return false;
			}

			node = this->CreateChildNode(node, expanding_move, board);
			return true;
		}
		else {
			// fully-expanded
			node->next_move_getter.reset(nullptr);
		}
	}

	node = this->FindBestChildToExpand(node->children);
#ifdef DEBUG
	if (node == nullptr) throw std::runtime_error("cannot find best child to expand");
#endif

	this->traversed_nodes.push_back(node); // back-propagate need to know the original node (not the redirected one)

	board.ApplyMove(node->move);

	if (node->equivalent_node != nullptr) {
		this->traversed_path.AddRedirectNode(node->equivalent_node);
		node = node->equivalent_node;
	}

	return false;
}

// return true if a new node is added; 'node' and 'board' will be the new node
// return false if an existing node is chosen; 'node' and 'board' will be the existing node
inline bool MCTS::ExpandNodeWithSingleRandomNextMove(TreeNode * & node, GameEngine::Board & board)
{
	// we only have one possible next move,
	// so we don't need to create redirect nodes
	// Note: redirect nodes are important in the MCTS selection phase,
	//       however, since we only have one possible next move,
	//       we re-generate a new game-flow move, and follow it

	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	bool prev_next_moves_are_deterministic = node->next_moves_are_deterministic;
#endif
	board.GetNextMoves(expanding_move, node->next_moves_are_deterministic);
#ifdef DEBUG
	if (node->next_moves_are_deterministic != prev_next_moves_are_deterministic) {
		throw std::runtime_error("parent->next_moves_are_random should not be altered.");
	}
#endif

	bool next_board_is_random;
	board.ApplyMove(expanding_move, &next_board_is_random);

	TreeNode * transposition_node = nullptr;

	if (next_board_is_random == false) {
		// the move is applied in a deterministic way
		// --> that is, all moves are deterministic
		// --> so, we have only one outcome

#ifdef DEBUG
		if (node->children.empty()) throw std::runtime_error("You should call ExpandNewNode() to expand at least once.");
		if (node->children.size() != 1) throw std::runtime_error("a deterministic game-flow move should only have one outcome (i.e., one child)!");
#endif

		transposition_node = node->children.front();

		this->traversed_nodes.push_back(transposition_node);
		if (transposition_node->equivalent_node != nullptr) {
			this->traversed_path.AddRedirectNode(transposition_node);
			transposition_node = transposition_node->equivalent_node;
		}
		node = transposition_node;
		return false;
	}

	// game-flow move is non-determinsitic for this board
	transposition_node = this->board_finder.Find(Turn::GetTurn(node, expanding_move), board, this->board_initializer.get());
	if (transposition_node) {
#ifdef DEBUG
		if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");
#endif

		this->traversed_nodes.push_back(transposition_node);
		this->traversed_path.AddHiddenRedirectNode(node, transposition_node);
		node = transposition_node;
		return false;
	}

	node = this->CreateChildNode(node, expanding_move, board);
	return true;
}

// return true if a new node is added; 'node' and 'board' will be the new node
// return false if an existing node is chosen; 'node' and 'board' will be the existing node
inline bool MCTS::ExpandNodeWithMultipleRandomNextMoves(TreeNode * & node, GameEngine::Board & board)
{
	GameEngine::Move & expanding_move = this->allocated_node->move;

#ifdef DEBUG
	if (!this->UseNextMoveGetter(node)) throw std::runtime_error("game-flow stages should with only one next move");
#endif

	GameEngine::NextMoveGetter next_move_getter;
	bool is_deterministic;
	board.GetNextMoves(next_move_getter, is_deterministic);
#ifdef DEBUG
	if (is_deterministic == true) throw std::runtime_error("this node should be with non-deterministic next moves");
#endif

	std::list<TreeNode*> children;
	while (true) {
		if (next_move_getter.GetNextMove(board, expanding_move) == false) break;

		TreeNode * child_found = node->FindChildByMove(expanding_move);
		if (child_found == nullptr) {
			// this move has not been expanded --> expand it
			board.ApplyMove(expanding_move);

			TreeNode *transposition_node = this->board_finder.Find(Turn::GetTurn(node, expanding_move), board, this->board_initializer.get());
			if (transposition_node) {
#ifdef DEBUG
				if (transposition_node->parent == node) {
					// expanded before under the same parent but with different move
					// --> no need to create a new redirect node
					throw std::runtime_error("two different moves under the same parent yield identical game states, why is this happening?");
					// Note: we can still create redirect node, but we want to know why this is happening
				}
				if (transposition_node->equivalent_node) throw std::runtime_error("logic error: 'board_node_map' should not store redirect nodes");

				for (auto const& child_node : node->children) {
					if (child_node->equivalent_node == transposition_node) {
						throw std::runtime_error("two different moves yield an identical board, why is this happening?");
					}
				}
#endif

				// create a redirect node
				TreeNode * redirect_node = this->CreateRedirectNode(node, expanding_move, transposition_node);
				this->traversed_nodes.push_back(redirect_node);
				this->traversed_path.AddRedirectNode(redirect_node);

				node = transposition_node;
				return false;
			}

			node = this->CreateChildNode(node, expanding_move, board);
			return true;
		}

		// this move is expanded before
		children.push_back(child_found);
	}

	node = this->FindBestChildToExpand(children);
#ifdef DEBUG
	if (node == nullptr) throw std::runtime_error("cannot find best child to expand");
#endif

	this->traversed_nodes.push_back(node); // back-propagate need to know the original node (not the redirected one)

	board.ApplyMove(node->move);

	if (node->equivalent_node != nullptr) {
		this->traversed_path.AddRedirectNode(node);
		node = node->equivalent_node;
	}

	return false;
}