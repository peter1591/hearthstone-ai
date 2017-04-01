#pragma once

namespace Cards
{
	struct Card_EX1_312 : SpellCardBase<Card_EX1_312> {
		Card_EX1_312() {
			onplay_handler.SetOnPlayCallback([](FlowControl::onplay::context::OnPlay const& context) {
				auto functor = [&](state::CardRef ref) {
					context.manipulate_.Minion(ref).Destroy();
				};
				context.manipulate_.Board().FirstPlayer().minions_.ForEach(functor);
				context.manipulate_.Board().SecondPlayer().minions_.ForEach(functor);
			});
		}
	};
}

REGISTER_CARD(EX1_312)