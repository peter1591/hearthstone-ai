#pragma once

namespace state
{
	namespace board
	{
		class PlayerResource
		{
		public:
			PlayerResource() : current_(0), total_(0) {}

			void SetTotal(int total)
			{
				assert(total <= 10);
				total_ = total;
			}
			
			int GetTotal() const { return total_; }
			void IncreaseTotal(int amount = 1)
			{
				int new_total = total_ + amount;
				if (new_total > 10) new_total = 10;
				SetTotal(new_total);
			}

			void SetCurrent(int current) { current_ = current; }
			int GetCurrent() const { return current_; }
			void IncreaseCurrent(int amount = 1) { current_ += amount; }

			void Refill() { current_ = total_; }
			void Cost(int amount) { current_ -= amount; }

		private:
			int current_;
			int total_;
		};
	}
}