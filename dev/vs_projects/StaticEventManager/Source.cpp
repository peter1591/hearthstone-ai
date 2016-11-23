#include "StaticEventManager/Events/GameStart.h"

int main(void)
{
	typedef StaticEventManager::Events::GameStartEvent GameStartEventType;

	GameStartEventType::Trigger();

	return 0;
}