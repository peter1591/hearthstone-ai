
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
			struct GetPlayCardCost {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef card_ref_;
					int * cost_;
					bool * cost_health_instead_;
				};
				using type = std::function<bool(Context)>;
			};
			struct AfterAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct AfterMinionPlayed {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
				};
				using type = std::function<bool(Context)>;
			};
			struct AfterMinionSummoned {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
				};
				using type = std::function<bool(Context)>;
			};
			struct AfterMinionDied {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
				};
				using type = std::function<bool(Context)>;
			};
			struct BeforeAttack {
				using type = bool(*)(CardRef, State &, CardRef);
			};
			struct BeforeMinionSummoned {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					CardRef card_ref_;
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
			struct OnPlay {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef card_ref_;
				};
				using type = std::function<bool(Context)>;
			};

			struct CalculateHealDamageAmount {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const source_ref_;
					int * amount_;
				};
				using type = std::function<bool(Context&&)>;
			};
			struct PrepareHealDamageTarget {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const source_ref_;
					state::CardRef * target_ref_; // an invalid target means the damage event is cancelled
				};
				using type = std::function<bool(Context&&)>;
			};
			struct OnTakeDamage {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					int damage_;
				};
				using type = bool(*)(state::CardRef, Context);
			};

			struct AfterHeroPower { // a.k.a. inspire
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef const card_ref_;
				};
				using type = bool(*)(Context);
			};

			struct AfterSpellPlayed {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::PlayerIdentifier player_;
					state::CardRef const card_ref_;
				};
				using type = std::function<bool(Context)>;
			};

			struct AfterSecretPlayed {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::PlayerIdentifier player_;
					state::CardRef const card_ref_;
				};
				using type = std::function<bool(Context)>;
			};

			struct OnHeal {
				struct Context {
					FlowControl::Manipulate & manipulate_;
					state::CardRef card_ref_;
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
				struct Context {
					FlowControl::Manipulate & manipulate_;
				};
				using type = std::function<bool(Context)>;
			};
		}
	}
}