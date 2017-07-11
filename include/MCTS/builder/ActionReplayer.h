#pragma once

namespace mcts
{
	namespace builder
	{
		class ActionReplayer
		{
		private:
			struct Item {
				ActionType action_type_; // TODO: debug only
				int choice_;

				Item(ActionType action_type, int choice) :
					action_type_(action_type), choice_(choice)
				{}
			};

		public:
			ActionReplayer() : choices_(), idx_(0), replay_failed_(false) {}

			int GetChoice(ActionType action_type) const {
				assert(action_type == choices_[idx_].action_type_);
				return choices_[idx_].choice_;
			}

			void RecordChoice(ActionType action_type, int choice) {
				assert(choice >= 0);
				choices_.emplace_back(action_type, choice);
			}
			
			void StepNext() {
				assert(idx_ < choices_.size());
				++idx_;
			}

			bool IsEnd() const { return idx_ >= choices_.size(); }

			void Restart() {
				idx_ = 0;
				replay_failed_ = false;
			}

			void Clear() {
				choices_.clear();
				idx_ = 0;
				replay_failed_ = false;
			}

			bool Empty() const {
				if (!choices_.empty()) return false;
				if (idx_ != 0) return false;
				return true;
			}

			void RemoveLast() {
				choices_.pop_back();
			}

		public:
			void MarkReplayFailed() { replay_failed_ = true; }
			bool IsReplayFailed() const { return replay_failed_; }

		private:
			using Container = std::vector<Item>;
			Container choices_;
			size_t idx_;

			bool replay_failed_;
		};
	}
}