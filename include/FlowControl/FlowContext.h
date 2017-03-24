#pragma once

#include <map>

#include "state/IRandomGenerator.h"
#include "state/Types.h"
#include "FlowControl/IActionParameterGetter.h"

namespace FlowControl {
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
		FlowContext(state::IRandomGenerator & random, IActionParameterGetter & action_parameters)
			: random_(random), action_parameters_(action_parameters),
			result_(FlowControl::kResultNotDetermined),
			minion_put_location_(-1)
		{}

	public: // action parameter
		void ResetActionParameter()
		{
			minion_put_location_ = -1;
			specified_target_.Invalidate();
		}

		auto GetMinionPutLocation(int min, int max) {
			if (minion_put_location_ < 0) {
				if (min >= max) {
					minion_put_location_ = min;
				}
				else {
					minion_put_location_ = action_parameters_.GetMinionPutLocation(min, max);
				}
				assert(minion_put_location_ >= 0);
			}
			return minion_put_location_;
		}

		void PrepareSpecifiedTarget(state::State & state, state::CardRef card_ref, const state::Cards::Card & card, state::targetor::Targets const& target_info);

		state::CardRef GetSpecifiedTarget()
		{
			if (specified_target_.IsValid()) return specified_target_;

			// TODO: maybe use this to indicate 'cast a spell with a random-choose target'
			assert(false);
			return state::CardRef();
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
		IActionParameterGetter & action_parameters_;
		std::multimap<int, state::CardRef> dead_entity_hints_;
		int minion_put_location_;
		state::CardRef specified_target_;
		state::CardRef destroyed_weapon_;
	};
}