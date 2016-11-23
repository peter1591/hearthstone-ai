#include "StaticEventManager/Manager.h"

int main(void)
{
	typedef StaticEventManager::Manager::GameStartEvent GameStartEventType;

	GameStartEventType::Trigger();

	return 0;
}