#include "InputManager.h"

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

		}
	}
}