#pragma once

#include <type_traits>
#include <utility>
#include "state/Types.h"
#include "state/Events/impl/HandlersContainer.h"
#include "state/Events/impl/CategorizedHandlersContainer.h"
#include "state/Events/EventTypes.h"

namespace state
{
	namespace Events
	{
		template <typename T> class Event;
		template <typename T> class CategorizedEvent;

		class Manager
		{
			template <typename T> friend class Event;
			template <typename T> friend class CategorizedEvent;

		public:
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Weffc++"
#endif
			Manager() : event_trigger_recursive_count_(0) {}
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
			template <typename EventType, typename T>
			void PushBack(T&& handler) {
				GetHandlersContainer<EventType>().PushBack(std::forward<T>(handler));
			}

			template <typename EventType, typename T>
			void PushBack(CardRef card_ref, T&& handler) {
				GetCategorizedHandlersContainer<EventType>().PushBack(
					card_ref, std::forward<T>(handler));
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerEvent(Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetHandlersContainer<EventTriggerType>().TriggerAll(std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

			template <typename EventTriggerType, typename... Args>
			void TriggerCategorizedEvent(CardRef card_ref, Args&&... args)
			{
				if (event_trigger_recursive_count_ >= max_event_trigger_recursive_) throw std::runtime_error("reach maximum event trigger recursive");
				++event_trigger_recursive_count_;
				GetCategorizedHandlersContainer<EventTriggerType>()
					.TriggerAll(card_ref, std::forward<Args>(args)...);
				--event_trigger_recursive_count_;
			}

		private:
			constexpr static int max_event_trigger_recursive_ = 100;
			int event_trigger_recursive_count_;

		private:
			template<typename EventHandlerType>
			impl::HandlersContainer<EventHandlerType> & GetHandlersContainer();

			template<typename EventHandlerType>
			impl::CategorizedHandlersContainer<EventHandlerType> & GetCategorizedHandlersContainer();

#define ADD_TRIGGER_TYPE_INTERNAL_INCLASS(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::HandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME;
#define ADD_TRIGGER_TYPE_INTERNAL_OUTCLASS(TYPE_NAME, MEMBER_NAME) \
	template <> inline impl::HandlersContainer<EventTypes::TYPE_NAME> & Manager::GetHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_TRIGGER_TYPE_INCLASS(TYPE_NAME) ADD_TRIGGER_TYPE_INTERNAL_INCLASS(TYPE_NAME, handler_ ## TYPE_NAME ## _)
#define ADD_TRIGGER_TYPE_OUTCLASS(TYPE_NAME) ADD_TRIGGER_TYPE_INTERNAL_OUTCLASS(TYPE_NAME, handler_ ## TYPE_NAME ## _)

#define ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_INCLASS(TYPE_NAME, MEMBER_NAME) \
private: \
	impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> MEMBER_NAME;
#define ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_OUTCLASS(TYPE_NAME, MEMBER_NAME) \
	template <> inline impl::CategorizedHandlersContainer<EventTypes::TYPE_NAME> & Manager::GetCategorizedHandlersContainer() { \
		return this->MEMBER_NAME; \
	}
#define ADD_CATEGORIZED_TRIGGER_TYPE_INCLASS(TYPE_NAME) ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_INCLASS(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)
#define ADD_CATEGORIZED_TRIGGER_TYPE_OUTCLASS(TYPE_NAME) ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_OUTCLASS(TYPE_NAME, categorized_handler_ ## TYPE_NAME ## _)

			ADD_TRIGGER_TYPE_INCLASS(GetPlayCardCost)

			ADD_TRIGGER_TYPE_INCLASS(AfterMinionSummoned)
			ADD_TRIGGER_TYPE_INCLASS(BeforeMinionSummoned)
			ADD_TRIGGER_TYPE_INCLASS(AfterMinionPlayed)
			ADD_TRIGGER_TYPE_INCLASS(AfterMinionDied)

			ADD_TRIGGER_TYPE_INCLASS(PreparePlayCardTarget)
			ADD_TRIGGER_TYPE_INCLASS(OnPlay)
			ADD_TRIGGER_TYPE_INCLASS(CheckPlayCardCountered)

			ADD_TRIGGER_TYPE_INCLASS(OnTurnEnd)
			ADD_TRIGGER_TYPE_INCLASS(OnTurnStart)

			ADD_TRIGGER_TYPE_INCLASS(BeforeSecretReveal)

			ADD_TRIGGER_TYPE_INCLASS(PrepareAttackTarget)
			ADD_TRIGGER_TYPE_INCLASS(BeforeAttack)
			ADD_TRIGGER_TYPE_INCLASS(AfterAttack)

			ADD_TRIGGER_TYPE_INCLASS(CalculateHealDamageAmount)
			ADD_TRIGGER_TYPE_INCLASS(PrepareHealDamageTarget)
			ADD_TRIGGER_TYPE_INCLASS(OnTakeDamage)
			ADD_CATEGORIZED_TRIGGER_TYPE_INCLASS(CategorizedOnTakeDamage)
			ADD_TRIGGER_TYPE_INCLASS(OnHeal)
			ADD_TRIGGER_TYPE_INCLASS(AfterTakenDamage)

			ADD_TRIGGER_TYPE_INCLASS(AfterHeroPower)
			ADD_TRIGGER_TYPE_INCLASS(AfterSpellPlayed)
			ADD_TRIGGER_TYPE_INCLASS(AfterSecretPlayed)

		};
			ADD_TRIGGER_TYPE_OUTCLASS(GetPlayCardCost)

			ADD_TRIGGER_TYPE_OUTCLASS(AfterMinionSummoned)
			ADD_TRIGGER_TYPE_OUTCLASS(BeforeMinionSummoned)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterMinionPlayed)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterMinionDied)

			ADD_TRIGGER_TYPE_OUTCLASS(PreparePlayCardTarget)
			ADD_TRIGGER_TYPE_OUTCLASS(OnPlay)
			ADD_TRIGGER_TYPE_OUTCLASS(CheckPlayCardCountered)

			ADD_TRIGGER_TYPE_OUTCLASS(OnTurnEnd)
			ADD_TRIGGER_TYPE_OUTCLASS(OnTurnStart)

			ADD_TRIGGER_TYPE_OUTCLASS(BeforeSecretReveal)

			ADD_TRIGGER_TYPE_OUTCLASS(PrepareAttackTarget)
			ADD_TRIGGER_TYPE_OUTCLASS(BeforeAttack)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterAttack)

			ADD_TRIGGER_TYPE_OUTCLASS(CalculateHealDamageAmount)
			ADD_TRIGGER_TYPE_OUTCLASS(PrepareHealDamageTarget)
			ADD_TRIGGER_TYPE_OUTCLASS(OnTakeDamage)
			ADD_CATEGORIZED_TRIGGER_TYPE_OUTCLASS(CategorizedOnTakeDamage)
			ADD_TRIGGER_TYPE_OUTCLASS(OnHeal)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterTakenDamage)

			ADD_TRIGGER_TYPE_OUTCLASS(AfterHeroPower)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterSpellPlayed)
			ADD_TRIGGER_TYPE_OUTCLASS(AfterSecretPlayed)

#undef ADD_TRIGGER_TYPE_INTERNAL_INCLASS
#undef ADD_TRIGGER_TYPE_INTERNAL_OUTCLASS
#undef ADD_TRIGGER_TYPE_INCLASS
#undef ADD_TRIGGER_TYPE_OUTCLASS
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_INCLASS
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INTERNAL_OUTCLASS
#undef ADD_CATEGORIZED_TRIGGER_TYPE_INCLASS
#undef ADD_CATEGORIZED_TRIGGER_TYPE_OUTCLASS
	}
}
