#pragma once

#include <stdexcept>
#include <string>
#include <unordered_set>

namespace ui
{
	class Decks
	{
	public:
		static std::unordered_multiset<std::string> GetDeck(std::string const& deck_name) {
			if (deck_name == "InnKeeperBasicMage") {
				return {
					"Arcane Explosion",
					"Arcane Intellect",
					"Arcane Missiles",
					"Bloodfen Raptor",
					"Boulderfist Ogre",
					"Fireball",
					"Murloc Raider",
					"Nightblade",
					"Novice Engineer",
					"Oasis Snapjaw",
					"Polymorph",
					"Raid Leader",
					"River Crocolisk",
					"Sen'jin Shieldmasta",
					"Wolfrider",
					"Arcane Explosion",
					"Arcane Intellect",
					"Arcane Missiles",
					"Bloodfen Raptor",
					"Boulderfist Ogre",
					"Fireball",
					"Murloc Raider",
					"Nightblade",
					"Novice Engineer",
					"Oasis Snapjaw",
					"Polymorph",
					"Raid Leader",
					"River Crocolisk",
					"Sen'jin Shieldmasta",
					"Wolfrider"
				};
			}

			throw std::runtime_error("invalid deck name");
		}
	};
}