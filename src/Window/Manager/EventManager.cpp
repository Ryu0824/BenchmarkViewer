#include "EventManager.h"

void EventManager::End()
{
	for (uint32_t i = 0;i < (uint32_t)EventID::None;++i)
	{
		std::unordered_map<EventID, IEvent*>::iterator Iter = Events.find((EventID)i);

		if (Iter != Events.end())
		{
			IEvent* EventInstance = Iter->second;

			if (EventInstance)
			{
				delete EventInstance;
				Events.erase(Iter);
			}
		}
	}
}