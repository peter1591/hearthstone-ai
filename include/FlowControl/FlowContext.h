#pragma once

#include <map>

#include "state/IRandomGenerator.h"
#include "state/Types.h"
#include "FlowControl/detail/ActionParameterWrapper.h"

namespace FlowControl {
	namespace Helpers {
		class Resolver;
	}

	enum Result
	{
		kResultNotDetermined,
		kResultFirstPlayerWin,
		kResultSecondPlayerWin,
		kResultDraw,
		kResultInvalid
	};

	class FlowContext
	{
	public:
		FlowContext(state::IRandomGenerator & random, IActionParameterGetter & action_getter)
			: random_(random), action_parameters_(action_getter),
			result_(FlowControl::kResultNotDetermined)
		{}

	public: // action parameter
		void ResetActionParameter() { action_parameters_.Clear(); }

		template <typename... Args>
		auto PrepareBattlecryTarget(Args&&... args) {
			battlecry_target_ = action_parameters_.GetBattlecryTarget(std::forward<Args>(args)...);
		}
		state::CardRef GetBattlecryTarget() const { return battlecry_target_; }
		void SetBattlecryTarget(state::CardRef target) { battlecry_target_ = target; }

		template <typename... Args>
		auto GetMinionPutLocation(Args&&... args) {
			return action_parameters_.GetMinionPutLocation(std::forward<Args>(args)...);
		}

	public: // dead entry hint
		void AddDeadEntryHint(state::State & state, state::CardRef ref);

		template <typename T>
		void ForEachDeadEntryHint(T&& functor) {
			for (auto const& item: dead_entity_hints_) {
				functor(item);
			}
		}

		void ClearDeadEntryHint() { dead_entity_hints_.clear(); }

		bool Empty() const;

	public:
		state::IRandomGenerator & GetRandom() { return random_; }

	public:
		void SetResult(Result v) { result_ = v; }
		Result GetResult() const { return result_; }

	public:
		void SetDestroyedWeapon(state::CardRef ref)
		{
			assert(destroyed_weapon_.IsValid() == false);
			destroyed_weapon_ = ref;
		}

		state::CardRef GetDestroyedWeapon() const { return destroyed_weapon_; }
		void ClearDestroyedWeapon() { destroyed_weapon_.Invalidate(); }

	private:
		Result result_;
		state::IRandomGenerator & random_;
		detail::ActionParameterWrapper action_parameters_;
		std::multimap<int, state::CardRef> dead_entity_hints_;
		state::CardRef battlecry_target_;
		state::CardRef destroyed_weapon_;
	};
}