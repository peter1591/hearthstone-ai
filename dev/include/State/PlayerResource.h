#pragma once

namespace State
{
	class PlayerResource
	{
	public:
		void SetTotal(int total) { total_ = total; }
		void SetCurrent(int current) { current_ = current; }

		void Refill() { current_ = total_; }
		void IncreaseTotal(int amount = 1) { total_ += amount; }
		void Cost(int amount) { current_ -= amount; }

		int GetCurrent() const { return current_; }

	private:
		int current_;
		int total_;
	};
}