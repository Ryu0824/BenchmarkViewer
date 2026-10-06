#pragma once

#include <Windows.h>

enum class EKEY
{
	/*VK_LBUTTON,
	VK_RBUTTON,
	END,*/
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
	InputManager() = default;
	~InputManager() = default;

	void CalculateMousePosition(HWND hWnd);

	POINT ClientMousePosition;/*
	int ButtonArray[(UINT32)EKEY::END];*/
};