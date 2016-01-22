#ifndef GAME_ENGINE_WEAPON_H
#define GAME_ENGINE_WEAPON_H

namespace GameEngine {

class Weapon
{
	public:
		int card_id;

		bool operator==(const Weapon &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			return true;
		}

		bool operator!=(const Weapon &rhs) const
		{
			return !(*this == rhs);
		}
};

} // namespace GameEngine

#endif
