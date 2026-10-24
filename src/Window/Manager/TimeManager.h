#pragma once

#include <chrono>

using Clock = std::chrono::steady_clock;

class TimeManager
{
public:
	static TimeManager& GetInstance()
	{
		static TimeManager Instance;
		return Instance;
	}

	void Begin();
	void Update();
	void End();

	float GetDeltaTime() const { return DeltaTime; }

	TimeManager(const TimeManager&) = delete;
	TimeManager(TimeManager&&) = delete;

	TimeManager& operator=(const TimeManager&) = delete;
	TimeManager& operator=(TimeManager&&) = delete;
private:
	float DeltaTime = 0.0f;

	Clock::time_point PrevTime;

	TimeManager() = default;
	~TimeManager() = default;
};