#include "TimeManager.h"
#include <chrono>

void TimeManager::Begin()
{
	PrevTime = Clock::now();
}

void TimeManager::Update()
{
	Clock::time_point CurrentTime = Clock::now();

	DeltaTime = std::chrono::duration<float>(CurrentTime - PrevTime).count();

	PrevTime = CurrentTime;
}

void TimeManager::End()
{

}