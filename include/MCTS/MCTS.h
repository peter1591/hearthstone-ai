#pragma once

#include "MCTS/Tree.h"

#include "MCTS/detail/EpisodeState.h"
#include "MCTS/detail/ChoiceWhiteList.h"
#include "MCTS/detail/Statistic.h"

namespace mcts
{
	class MCTS
	{
	public:
		template <typename StartBoardGetter>
		void Iterate(StartBoardGetter&& start_board_getter);

		int UserChooseAction(int exclusive_max);
		int RandomChooseAction(int exclusive_max);

		void PrintStatistic() {
			statistic_.PrintMessage();
		}

	private:
		int ActionCallback(int choices, bool random);

		int SelectAction(int choices, bool random);
		int SelectActionByRandom(int choices);
		int SelectActionByChoice(int choices);

		int Simulate(int choices, bool random);

		void SwitchToSimulationMode() {
			// We use a flag here, since we cannot switch to simulation mode
			// in sub-actions.
			flag_switch_to_simulation_ = true;
		}

	private:
		Tree tree_;
		detail::EpisodeState episode_state_;
		detail::ChoiceWhiteList choice_white_list_;
		detail::Statistic<> statistic_;

		bool flag_switch_to_simulation_;

		class LastNodeInfo {
		public:
			LastNodeInfo() : parent_(nullptr), action_(0), child_(nullptr) {}
			void Set(TreeNode* parent, TreeNode* child, int action) {
				parent_ = parent;
				child_ = child;
				action_ = action;
			}

			TreeNode* GetParent() const { return parent_; }
			TreeNode* GetChild() const { return child_; }
			int GetAction() const { return action_; }

			void RemoveNode() {
				assert(parent_->GetChild(action_) == child_);
				parent_->RemoveChild(action_);
			}

		private:
			TreeNode* parent_;
			int action_;
			TreeNode* child_;
		} last_node_;
	};
}