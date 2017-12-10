#pragma once

#include "state/Types.h"
#include "state/targetor/TargetsGenerator.h"

namespace state
{
	class State;
	namespace Cards { class Card; }
}

namespace FlowControl
{
	namespace deathrattle
	{
		namespace context {
			struct Deathrattle;
		}

		class Handler
		{
		public:
			typedef void DeathrattleCallback(context::Deathrattle const&);

			Handler() : base_deathrattles_(nullptr), deathrattles_() {}

			Handler(Handler const& rhs) : base_deathrattles_(rhs.base_deathrattles_), deathrattles_(rhs.deathrattles_) {}
			Handler & operator=(Handler const& rhs) {
				base_deathrattles_ = rhs.base_deathrattles_;
				deathrattles_ = rhs.deathrattles_;
				return *this;
			}

			void FillWithBase(Handler const& base) {
				assert(base.base_deathrattles_ == nullptr);
				base_deathrattles_ = &base.deathrattles_;
				deathrattles_.clear();
			}

			void Clear() {
				if (base_deathrattles_) {
					base_deathrattles_ = nullptr;
				}
				deathrattles_.clear();
			}

			void Add(DeathrattleCallback* deathrattle) {
				deathrattles_.push_back(deathrattle);
			}

			void TriggerAll(context::Deathrattle const& context) const {
				if (base_deathrattles_) {
					for (auto deathrattle : *base_deathrattles_) {
						(*deathrattle)(context);
					}
				}
				for (auto deathrattle : deathrattles_) {
					(*deathrattle)(context);
				}
			}

		private:
			typedef std::vector<DeathrattleCallback*> Deathrattles;
			Deathrattles const* base_deathrattles_;
			Deathrattles deathrattles_;
		};
	}
}