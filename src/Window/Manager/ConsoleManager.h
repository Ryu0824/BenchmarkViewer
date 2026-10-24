#pragma once

class ConsoleManager
{
public:
	void Begin();

	static ConsoleManager& GetInstance()
	{
		static ConsoleManager Instance;
		return Instance;
	}

	ConsoleManager(const ConsoleManager&) = delete;
	ConsoleManager(ConsoleManager&&) = delete;

	ConsoleManager& operator=(const ConsoleManager&) = delete;
	ConsoleManager& operator=(ConsoleManager&&) = delete;

private:
	ConsoleManager() = default;
	~ConsoleManager() = default;
};