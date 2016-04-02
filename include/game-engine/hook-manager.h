#pragma once

namespace GameEngine
{
	class Board;

	class HookManager
	{
	public:
		HookManager(Board & board);
		HookManager(HookManager const& rhs) = delete;
		HookManager(HookManager && rhs) = delete;
		HookManager & operator=(HookManager const& rhs) = delete;
		HookManager & operator=(HookManager && rhs) = delete;

	public: // hooks


	private:
		Board & board;
	};
}