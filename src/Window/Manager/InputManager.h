#pragma once

#include <Windows.h>
#include <array>

struct KeyState
{
	bool Down = false;
	bool Pressed = false;
	bool Released = false;
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

	bool IsPressed(unsigned int key) const { return (key < Keys.size()) && (Keys[key].Pressed); }
	bool IsReleased(unsigned int key) const { return (key < Keys.size()) && (Keys[key].Released); }
	bool IsDown(unsigned int key) const { return (key < Keys.size()) && (Keys[key].Down); }

private:
	void OnKeyDown(unsigned int key);
	void OnKeyUp(unsigned int key);
	void Reset();

private:
	InputManager() = default;
	~InputManager() = default;

	void CalculateMousePosition(HWND hWnd);

	POINT ClientMousePosition;
	std::array<KeyState, 256> Keys{};

	friend LRESULT CALLBACK WindowProc(HWND h, UINT m, WPARAM w, LPARAM l);
};