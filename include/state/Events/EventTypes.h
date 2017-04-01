
#include <tuple>
#include <string>
#include <iostream>
#include <functional>
#include "state/Types.h"

namespace FlowControl { class Manipulate; }
namespace state {
	class State;

	namespace Events {
		namespace EventTypes {
			struct AfterAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct AfterMinionPlayed {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
					Cards::Card const& card_;
				};
				using type = std::function<bool(Context)>;
			};
			struct AfterMinionSummoned {
				using type = bool(*)(CardRef, const Cards::Card &);
			};
			struct BeforeAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct BeforeMinionSummoned {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
					const Cards::Card & card_;
				};
				using type = bool(*)(Context);
			};
			struct OnAttack {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef defender_;
				};
				using type = std::function<bool(CardRef, Context)>;
			};
			struct OnMinionPlay {
				using type = bool(*)(const Cards::Card &);
			};

			struct CalculateHealDamageAmount {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const source_ref_;
					state::Cards::Card const& source_card_;
					int * amount_;
				};
				using type = std::function<bool(Context&&)>;
			};
			struct PrepareHealDamageTarget {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const source_ref_;
					state::Cards::Card const& source_card_;
					state::CardRef * target_ref_; // an invalid target means the damage event is cancelled
					state::Cards::Card const* target_card_;
				};
				using type = std::function<bool(Context&&)>;
			};
			struct OnTakeDamage {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::Cards::Card const& card_;
					int damage_;
				};
				using type = bool(*)(state::CardRef, Context);
			};

			struct AfterHeroPower { // a.k.a. inspire
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const card_ref_;
					state::Cards::Card const& card_;
				};
				using type = bool(*)(Context);
			};

			struct AfterSpell {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const card_ref_;
					state::Cards::Card const& card_;
				};
				using type = std::function<bool(Context)>;
			};

			struct OnHeal {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef card_ref_;
					state::Cards::Card const& card_;
					int amount_;
				};
				using type = std::function<bool(Context)>;
			};
			struct OnTurnEnd {
				struct Context {
					FlowControl::Manipulate & manipulate_;
				};
				using type = std::function<bool(Context)>;
			};
			struct OnTurnStart {
				using type = bool(*)();
			};
		}
	}
}