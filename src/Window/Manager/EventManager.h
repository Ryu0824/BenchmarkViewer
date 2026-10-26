#pragma once

#include <functional>
#include <unordered_map>

template<typename signature>
class Callback;

template<typename T, typename...Args>
class Callback<T(Args...)>
{
public:
	template<typename F>
	void Bind(F&& function)
	{
		Func = std::forward<F>(function);
	}

	T Execute(Args... args) const
	{
		return Func(std::forward<Args>(args)...);
	}

private:
	std::function<T(Args...)> Func;
};

class IEvent
{
public:
	virtual ~IEvent() = default;
};

template<typename signature>
class Event;

template<typename T, typename... Args>
class Event<T(Args...)> : public IEvent
{
public:
	void Subscribe(Callback<T(Args...)> callback) { Callbacks.push_back(std::move(callback)); }

	void BroadCast(Args... args)
	{
		for (auto& callback : Callbacks)
		{
			callback.Execute(args...);
		}
	}
private:
	std::vector<Callback<T(Args...)>> Callbacks;
};

enum class EventID
{
	WindowResize,
	None,
};

class EventManager
{
public:
	static EventManager& GetInstance()
	{
		static EventManager Instance;
		return Instance;
	}

	EventManager(const EventManager&) = delete;
	EventManager(EventManager&&) = delete;

	EventManager& operator=(const EventManager&) = delete;
	EventManager& operator=(EventManager&&) = delete;

	template<typename signature>
	Event<signature>& Create(EventID Id)
	{
		if (Events.contains(Id))
		{
			// Asssert 처리
		}

		Event<signature>* event = new Event<signature>();
		Events.emplace(std::make_pair(Id, event));
		return *event;
	}

	template<typename signature>
	Event<signature>& Get(EventID Id)
	{
		std::unordered_map<EventID, IEvent*>::iterator iter = Events.find(Id);

		if (iter == Events.end())
		{
			// Asssert 처리
		}

		Event<signature>* event = dynamic_cast<Event<signature>*>(iter->second);

		if (!event)
		{
			// Asssert 처리
		}

		return *event;
	}
	 
	void End();

private:
	EventManager() = default;
	~EventManager() = default;

	std::unordered_map<EventID, IEvent*> Events;
};