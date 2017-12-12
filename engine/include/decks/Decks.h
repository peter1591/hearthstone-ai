#pragma once

#include <stdexcept>
#include <string>
#include <unordered_set>

namespace decks
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

			if (deck_name == "InnKeeperBasicPaladin") {
				return {
					"Blessing of Might",
					"Gnomish Inventor",
					"Goldshire Footman",
					"Hammer of Wrath",
					"Hand of Protection",
					"Holy Light",
					"Ironforge Rifleman",
					"Light's Justice",
					"Lord of the Arena",
					"Nightblade",
					"Raid Leader",
					"Stonetusk Boar",
					"Stormpike Commando",
					"Stormwind Champion",
					"Stormwind Knight",
					"Blessing of Might",
					"Gnomish Inventor",
					"Goldshire Footman",
					"Hammer of Wrath",
					"Hand of Protection",
					"Holy Light",
					"Ironforge Rifleman",
					"Light's Justice",
					"Lord of the Arena",
					"Nightblade",
					"Raid Leader",
					"Stonetusk Boar",
					"Stormpike Commando",
					"Stormwind Champion",
					"Stormwind Knight"
				};
			}

			if (deck_name == "InnKeeperExpertShaman") {
				return {
					"Abusive Sergeant",
					"Abusive Sergeant",
					"Bloodlust",
					"Bloodlust",
					"Dire Wolf Alpha",
					"Dire Wolf Alpha",
					"Dust Devil",
					"Dust Devil",
					"Faerie Dragon",
					"Faerie Dragon",
					"Feral Spirit",
					"Feral Spirit",
					"Flametongue Totem",
					"Flametongue Totem",
					"Forked Lightning",
					"Forked Lightning",
					"Lava Burst",
					"Lava Burst",
					"Lightning Bolt",
					"Lightning Bolt",
					"Shattered Sun Cleric",
					"Shattered Sun Cleric",
					"Shieldbearer",
					"Shieldbearer",
					"Stormforged Axe",
					"Stormforged Axe",
					"Thrallmar Farseer",
					"Thrallmar Farseer",
					"Young Dragonhawk",
					"Young Dragonhawk"
				};
			}

			if (deck_name == "InnKeeperExpertWarlock") {
				return {
					"Blood Imp",
					"Blood Imp",
					"Dark Iron Dwarf",
					"Dark Iron Dwarf",
					"Demonfire",
					"Demonfire",
					"Doomguard",
					"Doomguard",
					"Dread Infernal",
					"Dread Infernal",
					"Flame Imp",
					"Flame Imp",
					"Imp Master",
					"Imp Master",
					"Knife Juggler",
					"Knife Juggler",
					"Mortal Coil",
					"Mortal Coil",
					"Power Overwhelming",
					"Power Overwhelming",
					"Sense Demons",
					"Sense Demons",
					"Shadow Bolt",
					"Shadow Bolt",
					"Shadowflame",
					"Shadowflame",
					"Succubus",
					"Succubus",
					"Voidwalker",
					"Voidwalker"
				};
			}

			throw std::runtime_error("invalid deck name");
		}
	};
}