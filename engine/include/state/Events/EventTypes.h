
#include <tuple>
#include <string>
#include <functional>
#include "state/Types.h"

namespace engine {
	namespace FlowControl { class Manipulate; }
}

namespace state {
	class State;

	namespace Events {
		namespace EventTypes {
			struct GetPlayCardCost {
				struct Context {
					state::State const& state_;
					state::CardRef card_ref_;
					int * cost_;
					bool * cost_health_instead_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct AfterMinionPlayed {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					CardRef card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct AfterMinionSummoned {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					CardRef card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct AfterMinionDied {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					CardRef card_ref_;
					state::PlayerIdentifier died_minion_owner_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct BeforeMinionSummoned {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					CardRef card_ref_;
				};
				using type = bool(*)(Context const&);
			};

			struct PrepareAttackTarget {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef attacker_;
					state::CardRef * defender_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct BeforeAttack {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef attacker_;
					state::CardRef defender_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct AfterAttack {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef attacker_;
					state::CardRef defender_;
				};
				using type = std::function<bool(Context const&)>;
			};
			
			struct PreparePlayCardTarget {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
					state::CardRef * target_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct OnPlay {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct CheckPlayCardCountered {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
					bool * countered_;
				};
				using type = std::function<bool(Context const&)>;
			};

			struct CalculateHealDamageAmount {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef const source_ref_;
					int * amount_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct PrepareHealDamageTarget {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef const source_ref_;
					state::CardRef * target_ref_; // an invalid target means the damage event is cancelled
				};
				using type = std::function<bool(Context const&)>;
			};

			struct OnTakeDamage {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
					int * damage_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct CategorizedOnTakeDamage {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					int * damage_;
				};
				using type = bool(*)(state::CardRef, Context const&);
			};
			struct AfterTakenDamage {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
					int damage_;
					int damage_after_armor_absorbed_;
				};
				using type = std::function<bool(Context const&)>;
			};

			struct AfterHeroPower { // a.k.a. inspire
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef const card_ref_;
				};
				using type = bool(*)(Context const&);
			};

			struct AfterSpellPlayed {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::PlayerIdentifier player_;
					state::CardRef const card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};

			struct AfterSecretPlayed {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::PlayerIdentifier player_;
					state::CardRef const card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};

			struct OnHeal {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
					int amount_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct OnTurnEnd {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
				};
				using type = std::function<bool(Context const&)>;
			};
			struct OnTurnStart {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
				};
				using type = std::function<bool(Context const&)>;
			};

			struct BeforeSecretReveal {
				struct Context {
					engine::FlowControl::Manipulate const& manipulate_;
					state::CardRef card_ref_;
				};
				using type = std::function<bool(Context const&)>;
			};
		}
	}
}