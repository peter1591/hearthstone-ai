#pragma once

namespace Manipulators
{
	class PlayerStateManipulator;
}

namespace State
{
	class PlayerState
	{
		friend Manipulators::PlayerStateManipulator;

	public:
		int GetHP() const { return hp_; }
		int GetMaxHP() const { return max_hp_; }
		int GetShields() const { return shields_; }

	private:
		int hp_;
		int max_hp_;
		int shields_;
	};
}