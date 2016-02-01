#ifndef GAME_ENGINE_CARDS_CARD_CS2_189
#define GAME_ENGINE_CARDS_CARD_CS2_189

#include <iostream>
#include "game-engine/card-id-map.h"

namespace GameEngine {
namespace Cards {

class Card_CS2_189
{
public:
	static constexpr int card_id = CARD_ID_CS2_189;

	// Elven Archer

	static bool BattleCry()
	{
		//std::cout << "battle CRY!!!!!" << std::endl;

		return true;
	}
};

} // namespace Cards
} // namespace GameEngine

#endif
