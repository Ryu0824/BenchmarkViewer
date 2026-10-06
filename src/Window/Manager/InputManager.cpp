#include "InputManager.h"

#ifdef _DEBUG
#include <iostream>
#endif

void InputManager::Begin()
{

}

void InputManager::Update(HWND hWnd)
{
	CalculateMousePosition(hWnd);
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
			printf("MousePos(x, y) : %d, %d", ClientMousePosition.x, ClientMousePosition.y);
#endif
		}
	}
}