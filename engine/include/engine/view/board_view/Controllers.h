#pragma once

#include <vector>
#include <map>

#include "Cards/id-map.h"
#include "engine/view/board_view/UnknownCards.h"

namespace engine {
	namespace view {
		namespace board_view {
			struct ControllerInfo {
				static constexpr int kDeckBlockId = -1;
				std::vector<Cards::CardId> deck_cards_;
				board_view::UnknownCardsSets unknown_cards_sets_;
				std::map<int, size_t> sets_indics_; // block id -> set idx

				void Reset() {
					deck_cards_.clear();
					sets_indics_.clear();
					unknown_cards_sets_.Reset();
				}
			};

			class Controllers {
			public:
				ControllerInfo & Get(int controller) {
					if (controller == 1) return first_;
					else {
						assert(controller == 2);
						return second_;
					}
				}

				void Reset() {
					first_.Reset();
					second_.Reset();
				}

			private:
				ControllerInfo first_;
				ControllerInfo second_;
			};
		}
	}
}