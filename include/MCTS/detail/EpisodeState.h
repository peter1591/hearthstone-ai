#pragma once

#include <vector>
#include "MCTS/Board.h"

namespace mcts
{
	namespace detail
	{
		class EpisodeState
		{
		public:
			enum Stage {
				kStageSelection,
				kStageSimulation
			};

		public:
			EpisodeState() : stage_(kStageSelection), tree_node_(nullptr), is_valid_(true) {}

			void Start(Board const& board, TreeNode* tree_node)
			{
				path_.clear();

				board_ = board;
				tree_node_ = tree_node;
				assert(tree_node_);
				path_.push_back(tree_node_);
			}

			void StepNext(int action, TreeNode* next_node)
			{
				tree_node_ = next_node;
				assert(tree_node_);
				path_.push_back(tree_node_);
			}

			Stage GetStage() const { return stage_; }
			void SetToSimulationStage() { stage_ = kStageSimulation; }

			Board const& GetBoard() const { return board_; }
			Board & GetBoard() { return board_; }

			TreeNode* GetTreeNode() const { return tree_node_; }

			std::vector<TreeNode*> const& GetTraversedPath() const { return path_; }

			void SetInvalid() { is_valid_ = false; }
			bool IsValid() const { return is_valid_; }

		private:
			Board board_;
			Stage stage_;
			TreeNode* tree_node_;
			std::vector<TreeNode*> path_;
			bool is_valid_;
		};
	}
}