#pragma once

#include <functional>

class EventManager
{
public:
	template<typename T>
	using Callback = std::function<T>

	static EventManager& GetInstance()
	{
		static EventManager Instance;
		return Instance;
	}

	EventManager(const EventManager&) = delete;
	EventManager(EventManager&&) = delete;

	EventManager& operator=(const EventManager&) = delete;
	EventManager& operator=(EventManager&&) = delete;
	
	template<typename T, typename... ARGS>
	void AddEvent(std::function<T> callback)
	{ 
		
	}

	void BroadCast();

private:
	EventManager() = default;
	~EventManager() = default;

	// Message 모아놓고 실행할 객체 만들고
	std::vector<Callback> Callbacks;
 };