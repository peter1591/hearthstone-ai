#pragma once

namespace mcts
{
	namespace builder
	{
		class ActionReplayer
		{
		private:
			struct Item {
				engine::ActionType action_type_; // TODO: debug only
				int choice_;

				Item() : action_type_(), choice_(-1) {}
				Item(engine::ActionType action_type, int choice) :
					action_type_(action_type), choice_(choice)
				{}
			};

		public:
			ActionReplayer() : choices_(), idx_(0) {}

			int GetChoice(engine::ActionType action_type) const {
				assert(action_type == choices_[idx_].action_type_);
				return choices_[idx_].choice_;
			}

			void RecordChoice(engine::ActionType action_type, int choice) {
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
			}

			void Clear() {
				choices_.clear();
				idx_ = 0;
			}

			bool Empty() const {
				if (!choices_.empty()) return false;
				if (idx_ != 0) return false;
				return true;
			}

			void RemoveLast(int count = 1) {
				assert(count >= 1);
				assert(choices_.size() >= (size_t)count);
				choices_.resize(choices_.size() - (size_t)count);
			}

		private:
			using Container = std::vector<Item>;
			Container choices_;
			size_t idx_;
		};
	}
}