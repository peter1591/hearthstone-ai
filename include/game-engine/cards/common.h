#ifndef GAME_ENGINE_CARDS_COMMON_H
#define GAME_ENGINE_CARDS_COMMON_H

#include <type_traits>
#include "game-engine/board.h"
#include "game-engine/move.h"

#include "card_CS2_188.h"
#include "card_CS2_189.h"
#include "card_FP1_007.h"
#include "card_FP1_002.h"

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
			Card:: ## MethodName(params...); \
			return true; \
		} \
		template <typename TestResult, typename... Params> \
		static bool CallInternal(std::enable_if_t<(sizeof(TestResult) != sizeof(test_ret_if_yes))>*, Params&&...) \
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
	static bool OnSummon(int card_id, GameEngine::Board const& board, SlotIndex side, SlotIndex put_location, GameEngine::BoardObjects::Minion & summoning_minion)
	{
		return CardCallbackManager::HandleCallback<Callback_OnSummon>(card_id, board, side, put_location, summoning_minion);
	}

	static bool GetRequiredTargets(int card_id, GameEngine::Board const& board, SlotIndex side, SlotIndexBitmap &targets, bool & meet_requirements)
	{
		return CardCallbackManager::HandleCallback<Callback_GetRequiredTargets>(card_id, board, side, targets, meet_requirements);
	}

	static bool BattleCry(int card_id, GameEngine::Board &board, GameEngine::Move::PlayMinionData const& play_minion_data)
	{
		return CardCallbackManager::HandleCallback<Callback_BattleCry>(card_id, board, play_minion_data);
	}

private:
	template <typename Card, typename Callback> struct CardCallbackCaller {};

	DECLARE_CARD_CALLBACK(OnSummon)
	DECLARE_CARD_CALLBACK(BattleCry)
	DECLARE_CARD_CALLBACK(GetRequiredTargets)

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

HANDLE_CARD_CALLBACK(Card_CS2_188)
HANDLE_CARD_CALLBACK(Card_CS2_189)
HANDLE_CARD_CALLBACK(Card_FP1_007)
HANDLE_CARD_CALLBACK(Card_FP1_002)

#undef HANDLE_CARD_CALLBACK
	default:
		return false; // no any special mechanism for this card
	}
}

}; // namespace Cards
}; // namespace GameEngine

#endif
