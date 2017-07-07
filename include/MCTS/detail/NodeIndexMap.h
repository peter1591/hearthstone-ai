#pragma once

#include <vector>

namespace mcts
{
	namespace detail
	{
		class NodeIndexMap
		{
		public:
			NodeIndexMap() : idx_map_() {}

			void PushBack(size_t idx) {
				idx_map_.push_back(idx);
			}

			bool Empty() const { return idx_map_.empty(); }
			size_t Size() const { return idx_map_.size(); }

			size_t Get(size_t idx) const {
				assert(idx < idx_map_.size());
				return idx_map_[idx];
			}

			template <typename Functor>
			void ForEach(Functor&& functor) const {
				for (size_t idx : idx_map_) {
					if (!functor(idx)) return;
				}
			}

			bool Erase(size_t idx) {
				for (auto it = idx_map_.begin(); it != idx_map_.end(); ++it) {
					if (*it == idx) {
						idx_map_.erase(it);
						return true;
					}
				}
				return false;
			}

			void Clear() {
				idx_map_.clear();
			}

		private:
			// map the 'index to valid children' to the 'index to all (including removed) children'
			std::vector<size_t> idx_map_;
		};
	}
}