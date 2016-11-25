#pragma once

namespace state
{
	class PlayerResource
	{
	public:
		void SetTotal(int total) { total_ = total; }
		int GetTotal() const { return total_; }

		void SetCurrent(int current) { current_ = current; }
		int GetCurrent() const { return current_; }

		void Refill() { current_ = total_; }
		void IncreaseTotal(int amount = 1) { total_ += amount; }
		void Cost(int amount) { current_ -= amount; }

	private:
		int current_;
		int total_;
	};
}