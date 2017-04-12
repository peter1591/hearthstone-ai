#pragma once

namespace state
{
	namespace board
	{
		class PlayerResource
		{
		public:
			PlayerResource() : current_(0), total_(0), overload_current_(0), overload_next_(0) {}

			void SetTotal(int total)
			{
				assert(total <= 10);
				total_ = total;
			}
			
			int GetTotal() const { return total_; }

			void GainCrystal(int amount = 1) {
				int new_total = total_ + amount;
				if (new_total > 10) new_total = 10;
				int gain = new_total - total_;
				SetTotal(new_total);
				current_ += gain;
			}

			void GainEmptyCrystal(int amount = 1) {
				int new_total = total_ + amount;
				if (new_total > 10) new_total = 10;
				SetTotal(new_total);
			}

			void Refill() { current_ = total_; }
			void SetCurrent(int current) { current_ = current; }
			int GetCurrent() const { return current_; }

			int GetCurrentOverloaded() const { return overload_current_; }
			void SetCurrentOverloaded(int v) { overload_current_ = v; }

			int GetNextOverload() const { return overload_next_; }
			void IncreaseNextOverload(int v) { overload_next_ += v; }

			void UnlockOverload() {
				current_ += overload_current_;
				overload_current_ = 0;
				overload_next_ = 0;
			}

			void Cost(int amount) {
				assert(GetCurrent() >= amount);
				if (amount < 0) return;
				current_ -= amount;
			}

			void TurnStart() {
				if (total_ < 10) {
					++total_;
				}
				overload_current_ = overload_next_;
				overload_next_ = 0;
				current_ = total_ - overload_current_;
			}

		private:
			int current_;
			int total_;
			int overload_current_;
			int overload_next_;
		};
	}
}