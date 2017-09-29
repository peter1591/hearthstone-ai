#pragma once

namespace state
{
	// Currently, the ordering of cards in hand does not have any impact
	// Even 'Simulacrum' card choose card randomly
	// --> https://hearthstone.gamepedia.com/Simulacrum
	// The good side is to reduce the useless branch factor in the game tree
	static constexpr bool kOrderHandCardsByCardId = true;
}