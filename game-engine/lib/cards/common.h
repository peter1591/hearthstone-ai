#ifndef GAME_ENGINE_CARDS_COMMON_H
#define GAME_ENGINE_CARDS_COMMON_H

#include <type_traits>
#include "game-engine/board.h"
#include "card_CS2_189.h"

namespace GameEngine {
	namespace Cards {

#define DECLARE_CARD_CALLBACK(MethodName) \
	struct Callback_ ## MethodName {}; \
	template <typename Card> \
	struct CardCallbackCaller<Card, Callback_ ## MethodName> \
	{ \
	private: \
		typedef char test_ret_if_yes; \
		typedef long test_ret_if_no; \
		template <class Card, typename T = decltype(&Card:: ## MethodName)> \
		struct TestResultWrapper { \
			typedef test_ret_if_yes type; \
		}; \
		template <typename T> static typename TestResultWrapper<T>::type TestMethodExists(void*); \
			template <typename T> static test_ret_if_no TestMethodExists(...); \
			template <typename TestResult, typename... Params> \
			static bool CallInternal(std::enable_if_t<(sizeof(TestResult) == sizeof(test_ret_if_yes))>*, Params&&... params) \
		{ \
			return Card:: ## MethodName(params...); \
		} \
		template <typename TestResult, typename... Params> \
		static bool CallInternal(std::enable_if_t<(sizeof(TestResult) != sizeof(test_ret_if_yes))>*, Params&&... params) \
		{ \
			return false; \
		} \
	public: \
		template <typename... Params> static bool Call(Params&&... params) \
		{ \
			typedef decltype(TestMethodExists<Card>(nullptr)) TestResult; \
			return CallInternal<TestResult, Params...>(nullptr, params...); \
		} \
	};

class CardCallbackManager
{
public:
	static bool BattleCry(int card_id, GameEngine::Board &board)
	{
		return CardCallbackManager::HandleCallback<Callback_BattleCry>(card_id);
	}

private:
	template <typename Card, typename Callback> struct CardCallbackCaller {};

	DECLARE_CARD_CALLBACK(BattleCry)

#undef DECLARE_CARD_CALLBACK

private:
	template <typename Callback, typename... Params>
	static bool HandleCallback(int card_id, Params&&... params);
};

template <typename Callback, typename ...Params>
inline bool CardCallbackManager::HandleCallback(int card_id, Params&&... params)
{
	switch (card_id)
	{
#define HANDLE_CARD_CALLBACK(CardClassName) \
	case CardClassName::card_id: \
		return CardCallbackCaller<CardClassName, Callback>::Call(params...);

HANDLE_CARD_CALLBACK(Card_CS2_189)

#undef HANDLE_CARD_CALLBACK
	default:
		return false; // no any special mechanism for this card
	}
}

}; // namespace Cards
}; // namespace GameEngine

#endif
