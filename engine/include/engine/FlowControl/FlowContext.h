#pragma once

#include <map>

#include "state/Types.h"
#include "engine/Result.h"
#include "engine/FlowControl/IActionParameterGetter.h"
#include "engine/FlowControl/IRandomGenerator.h"
#include "engine/FlowControl/detail/Resolver.h"

namespace engine {
	namespace FlowControl {
		class FlowContext
		{
		public:
			FlowContext() :
				result_(engine::kResultNotDetermined),
				action_parameters_(nullptr), random_(nullptr),
				dead_entity_hints_(),
				minion_put_location_(-1),
				specified_target_(), destroyed_weapon_(),
				user_choice_(Cards::kInvalidCardId),
				targets_(),
				resolver_()
			{}

			FlowContext(IRandomGenerator & random, IActionParameterGetter & action_parameters) :
				result_(engine::kResultNotDetermined),
				action_parameters_(&action_parameters), random_(&random),
				dead_entity_hints_(),
				minion_put_location_(-1),
				specified_target_(), destroyed_weapon_(),
				user_choice_(Cards::kInvalidCardId),
				targets_(),
				resolver_()
			{}

			FlowContext(FlowContext const&) = default;
			FlowContext & operator=(FlowContext const&) = default;

			void SetCallback(IRandomGenerator & random, IActionParameterGetter & action_parameters) {
				random_ = &random;
				action_parameters_ = &action_parameters;
			}

			void Reset()
			{
				result_ = engine::kResultNotDetermined;
				ClearDeadEntryHint();
				minion_put_location_ = -1;
				specified_target_.Invalidate();
				ClearDestroyedWeapon();
				user_choice_ = Cards::kInvalidCardId;
			}

		public: // action parameter
			auto GetMainOp() {
				return action_parameters_->ChooseMainOp();
			}

			auto ChooseHandCard() { return action_parameters_->ChooseHandCard(); }

			auto GetAttacker() { return action_parameters_->GetAttacker(); }

			state::CardRef GetDefender(std::vector<int> const& defenders) {
				assert(!defenders.empty());
				return action_parameters_->GetDefender(defenders);
			}

			auto GetMinionPutLocation(int minions) {
				if (minion_put_location_ < 0) {
					if (minions == 0) {
						minion_put_location_ = 0;
					}
					else {
						minion_put_location_ = action_parameters_->GetMinionPutLocation(minions);
					}
					assert(minion_put_location_ >= 0);
					assert(minion_put_location_ <= minions);
				}
				return minion_put_location_;
			}

			void PrepareSpecifiedTarget(state::State & state, state::CardRef card_ref, state::targetor::Targets const& target_info);
			void ChangeSpecifiedTarget(state::CardRef target) { specified_target_ = target; }
			state::CardRef GetSpecifiedTarget() const { return specified_target_; }

			Cards::CardId UserChooseOne(std::vector<Cards::CardId> const& cards) {
				if (cards.size() <= 1) return cards[0];
				return action_parameters_->ChooseOne(cards);
			}

			Cards::CardId GetSavedUserChoice() const { return user_choice_; }
			void SaveUserChoice(Cards::CardId choice) { user_choice_ = choice; }

		public: // dead entry hint
			void AddDeadEntryHint(state::State & state, state::CardRef ref);

			template <typename T>
			void ForEachDeadEntryHint(T&& functor) {
				for (auto const& item : dead_entity_hints_) {
					functor(item);
				}
			}

			void ClearDeadEntryHint() { dead_entity_hints_.clear(); }

			bool Empty() const;

		public:
			IRandomGenerator & GetRandom() { return *random_; }

		public:
			void SetResult(engine::Result v) { result_ = v; }
			engine::Result GetResult() const { return result_; }

		public:
			void SetDestroyedWeapon(state::CardRef ref)
			{
				assert(destroyed_weapon_.IsValid() == false);
				destroyed_weapon_ = ref;
			}

			state::CardRef GetDestroyedWeapon() const { return destroyed_weapon_; }
			void ClearDestroyedWeapon() { destroyed_weapon_.Invalidate(); }

		public:
			auto & GetResolver() { return resolver_; }

		private:
			engine::Result result_;
			IActionParameterGetter * action_parameters_;
			IRandomGenerator * random_;
			std::multimap<int, state::CardRef> dead_entity_hints_;
			int minion_put_location_;
			state::CardRef specified_target_;
			state::CardRef destroyed_weapon_;
			Cards::CardId user_choice_;
			std::vector<state::CardRef> targets_;
			detail::Resolver resolver_;
		};
	}
}