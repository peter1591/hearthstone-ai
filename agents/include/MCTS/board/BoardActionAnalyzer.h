#pragma once

#include <shared_mutex>
#include "MCTS/Types.h"
#include "FlowControl/PlayerStateView.h"
#include "FlowControl/ValidActionAnalyzer.h"
#include "Utils/SpinLocks.h"

namespace mcts
{
	namespace board
	{
		class BoardActionAnalyzer
		{
		public:
			BoardActionAnalyzer() : mutex_(), analyzer_() {}

			BoardActionAnalyzer(BoardActionAnalyzer const& rhs) :
				mutex_(), analyzer_(rhs.analyzer_)
			{}

			BoardActionAnalyzer & operator=(BoardActionAnalyzer const& rhs) {
				analyzer_ = rhs.analyzer_;
				return *this;
			}

			void Reset() { analyzer_.Reset(); }

			void Prepare(FlowControl::CurrentPlayerStateView const& board) { analyzer_.Analyze(board.GetValidActionGetter()); }

			auto GetMainActionsCount() {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				return analyzer_.GetMainActionsCount();
			}

			template <class Functor>
			void ForEachMainOp(Functor && functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				analyzer_.ForEachMainOp(std::forward<Functor>(functor));
			}
			FlowControl::MainOpType GetMainOpType(size_t choice) const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				return analyzer_.GetMainOpType(choice);
			}
			template <class Functor>
			void ForEachPlayableCard(Functor && functor) const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				analyzer_.ForEachPlayableCard(std::forward<Functor>(functor));
			}
			size_t GetPlaybleCard(size_t idx) const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				return analyzer_.GetPlayableCards()[idx];
			}

			auto GetPlayableCards() const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				auto ret = analyzer_.GetPlayableCards();
				return ret; // return by copy to ensure locking protection
			}

			int GetEncodedAttackerIndex(size_t idx) const {
				std::shared_lock<Utils::SharedSpinLock> lock(mutex_);
				return analyzer_.GetAttackers()[idx];
			}

		private:
			mutable Utils::SharedSpinLock mutex_;
			FlowControl::ValidActionAnalyzer analyzer_;
		};
	}
}