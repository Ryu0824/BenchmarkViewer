#include "InputManager.h"

#ifdef _DEBUG
#include <iostream>
#include "TimeManager.h"
#endif

void InputManager::Begin()
{
	for (KeyState& key : Keys)
	{
		key.Pressed = false;
		key.Released = false;
	}
}

void InputManager::Update(HWND hWnd)
{
	CalculateMousePosition(hWnd);
}


void InputManager::End()
{

}

void InputManager::OnKeyDown(unsigned int key)
{
	if (key >= Keys.size())return;

	KeyState& State = Keys[key];

	if (!State.Down)
		State.Pressed = true;

	State.Down = true;
}

void InputManager::OnKeyUp(unsigned int key)
{
	if (key >= Keys.size())return;

	KeyState& State = Keys[key];

	if (State.Down)
		State.Released = true;

	State.Down = false;
}

void InputManager::Reset()
{
	Keys = {};
}

void InputManager::CalculateMousePosition(HWND hWnd)
{
	if (GetCursorPos(&ClientMousePosition))
	{
		if (ScreenToClient(hWnd, &ClientMousePosition))
		{
#ifdef _DEBUG
			static float Duration = 0.0f;

			Duration += TimeManager::GetInstance().GetDeltaTime();

			if (Duration >= 5.0f)
			{
				printf("MousePos(x, y) : %d, %d\n", ClientMousePosition.x, ClientMousePosition.y);
				Duration = 0.0f;
			}
#endif
		}
	}
}