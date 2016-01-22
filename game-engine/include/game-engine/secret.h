#ifndef GAME_ENGINE_SECRET_H
#define GAME_ENGINE_SECRET_H

namespace GameEngine {

class Secret
{
	public:
		int card_id;

		bool operator==(const Secret &rhs) const
		{
			if (this->card_id != rhs.card_id) return false;
			return true;
		}

		bool operator!=(const Secret &rhs) const
		{
			return !(*this == rhs);
		}
};

} // namespace GameEngine

#endif
