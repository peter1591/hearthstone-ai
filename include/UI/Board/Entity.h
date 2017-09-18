#pragma once

#include "Cards/id-map.h"

#include "UI/Board/UnknownCards.h"

namespace ui
{
	namespace board
	{
		class Entity
		{
		public:
			Entity() : 
				id_(-1),
				controller_(-1),
				card_id_(Cards::kInvalidCardId),
				unknown_cards_set_id_(0), unknown_cards_set_card_idx_(0),
				resources_(0), temp_resources_(0), resources_used_(0), overload_locked_(0), overload_owed_(0)
			{}

			void ParseTags(std::unordered_map<std::string, int> const& tags) {
				resources_ = GetTagOrDefault(tags, "RESOURCES", -1);
			}

		private:
			int GetTagOrDefault(std::unordered_map<std::string, int> const& tags, std::string const& key, int v) {
				auto it = tags.find(key);
				if (it == tags.end()) return v;
				return it->second;
			}

		public:
			int id_;

			int controller_;
			Cards::CardId card_id_;
			size_t unknown_cards_set_id_;
			size_t unknown_cards_set_card_idx_;

			int resources_;
			int temp_resources_;
			int resources_used_;
			int overload_locked_;
			int overload_owed_;
		};
	}
}