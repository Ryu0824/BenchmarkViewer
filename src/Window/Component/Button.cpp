#include "Button.h"
#include "../Manager/InputManager.h"

void Button::Begin()
{

}

void Button::Update()
{
	// Mosue Button In
	if (Interactive())
	{
		if (State == ButtonState::None || State == ButtonState::OnMouseExit)
		{
			State = ButtonState::OnMouseEnter
		}
		else if (State == ButtonState::OnMouseEnter)
		{
			State = ButtonState::OnMouseOver;
		}
	}
	else
	{

	}
}

void Button::Render()
{

}

void Button::End()
{

}

bool Button::Interactive()
{
	POINT MousePos = InputManager::GetInstance().GetMousePosition();


}