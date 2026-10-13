#include "InputManager.h"

#ifdef _DEBUG
#include <iostream>
#include "TimeManager.h"
#endif

void InputManager::Begin()
{

}

void InputManager::Update(HWND hWnd)
{
	CalculateMousePosition(hWnd);

	GetAsyncKeyState
}

void InputManager::End()
{

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