#pragma once

#include <vector>

#include "Cards/Database.h"
#include "Cards/id-map.h"
#include "Cards/CardDispatcher.h"

namespace Cards
{
	class PreIndexedCards
	{
	public:
		enum IndexedType {
			kCollectibles,
			kMinionDemons,
			kMinionTaunt,
			kCachedCardsTypesCount // should be at last
		};

		static PreIndexedCards & GetInstance() {
			static PreIndexedCards instance;
			return instance;
		}

		void Initialize() {
			Database::GetInstance().ForEachCard([&](Database::CardData const& card) {
				ProcessCachedCardsTypes(card);
				return true;
			});
		}

		std::vector<int> const& GetIndexedCards(IndexedType type) {
			return indexed_cards_[type];
		}

	private:
		PreIndexedCards() : indexed_cards_() {}

		void ProcessCachedCardsTypes(Database::CardData const& new_card) {
			state::Cards::CardData card_data;
			try {
				card_data = CardDispatcher::CreateInstance((CardId)new_card.card_id);
			}
			catch (std::exception const&) {
				return;
			}

			if (new_card.collectible) {
				indexed_cards_[kCollectibles].push_back(new_card.card_id);
			}

			if (new_card.card_race == state::kCardRaceDemon &&
				new_card.card_type == state::kCardTypeMinion)
			{
				indexed_cards_[kMinionDemons].push_back(new_card.card_id);
			}

			if (new_card.card_type == state::kCardTypeMinion &&
				card_data.taunt)
			{
				indexed_cards_[kMinionTaunt].push_back(new_card.card_id);
			}
		}

	private:
		std::array<std::vector<int>, kCachedCardsTypesCount> indexed_cards_;
	};
}