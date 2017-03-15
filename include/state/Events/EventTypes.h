
#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "state/Events/HandlersContainerController.h"
#include "state/Types.h"

namespace FlowControl { class FlowContext; }
namespace state {
	class State;

	namespace Events {
		namespace EventTypes {
			struct AfterAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct AfterMinionPlayed {
				using type = bool(*)(const Cards::Card &);
			};
			struct AfterMinionSummoned {
				using type = bool(*)();
			};
			struct BeforeAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct BeforeMinionSummoned {
				struct Context {
					state::State & state_;
					state::CardRef card_ref_;
					const state::Cards::Card & card_;
				};
				using type = bool(*)(Context&&);
			};
			struct OnAttack {
				struct Context {
					state::State & state_;
					FlowControl::FlowContext & flow_context_;
					state::CardRef defender_;
				};
				using type = std::function<bool(CardRef, Context&&)>;
			};
			struct OnHeal {
				struct Context {
					state::State & state_;
					state::Cards::Card const& card_;
					int amount_;
				};
				using type = bool(*)(state::CardRef, Context&);
			};
			struct OnMinionPlay {
				using type = bool(*)(const Cards::Card &);
			};
			struct OnTakeDamage {
				struct Context {
					state::State & state_;
					FlowControl::FlowContext & flow_context_;
					state::Cards::Card const& card_;
					int damage_;
				};
				using type = bool(*)(state::CardRef, Context&);
			};
			struct OnTurnEnd {
				struct Context {
					state::State & state_;
					FlowControl::FlowContext & flow_context_;
				};
				using type = std::function<bool(Context &&)>;
			};
			struct OnTurnStart {
				using type = bool(*)();
			};
		}
	}
}