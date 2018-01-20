#pragma once

namespace alphazero
{
	namespace evaluation
	{
		class CompetitionResult
		{
		public:
			CompetitionResult() :
				total_games(0),
				competitor_wins(0)
			{}

			void Clear() {
				total_games = 0;
				competitor_wins = 0;
			}

			// Thread safe
			void AddResult(bool win) {
				++total_games;
				if (win) ++competitor_wins;
			}

			int GetTotal() const { return total_games.load(); }
			int GetWin() const { return competitor_wins.load(); }

		private:
			std::atomic<int> total_games;
			std::atomic<int> competitor_wins;
		};
	}
}