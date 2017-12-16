#pragma once

#include <vector>
#include "state/Types.h"
#include "state/targetor/Targets.h"
#include "Cards/id-map.h"
#include "FlowControl/MainOp.h"

namespace state {
	class State;
}

namespace FlowControl {
	class IActionParameterGetter
	{
	public:
		IActionParameterGetter() {}
		virtual ~IActionParameterGetter() {}
		IActionParameterGetter(IActionParameterGetter const&) = delete;
		IActionParameterGetter & operator=(IActionParameterGetter const&) = delete;

		static constexpr size_t kMaxChoices = 16; // max #-of-choices: choose target

		virtual MainOpType ChooseMainOp() = 0;

		virtual int ChooseHandCard() = 0;

		virtual state::CardRef GetAttacker() = 0;
		virtual state::CardRef GetDefender(std::vector<state::CardRef> const& targets) = 0;

		virtual int GetMinionPutLocation(int minions) = 0;

		// spell target
		virtual state::CardRef GetSpecifiedTarget(
			state::State & state, state::CardRef card_ref,
			std::vector<state::CardRef> const& targets) = 0;

		virtual Cards::CardId ChooseOne(std::vector<Cards::CardId> const& cards) = 0;
	};
}