#pragma once

#include <Windows.h>
#include <array>

struct ButtonState
{
	bool Down = false;
	bool IsPressed = false;
	bool IsReleased = false;
};

class InputManager
{
public:
	static InputManager& GetInstance()
	{
		static InputManager Instance;
		return Instance;
	}

	void Begin();
	void Update(HWND hWnd);
	void End();

	POINT GetMousePosition() const { return ClientMousePosition; }

	InputManager(const InputManager& Other) = delete;
	InputManager(InputManager&& Other) = delete;

	InputManager& operator=(const InputManager& Other) = delete;
	InputManager& operator=(InputManager&& Other) = delete;

private:
	void IsKeyDown(uint32_t key);
	void IsKeyUp(uint32_t key);

private:
	InputManager() = default;
	~InputManager() = default;

	void CalculateMousePosition(HWND hWnd);

	POINT ClientMousePosition;
	std::array<ButtonState, 256> ButtonStates;

	friend LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l);
};