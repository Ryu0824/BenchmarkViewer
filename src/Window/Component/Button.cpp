#include "Button.h"
#include "../Manager/InputManager.h"
#include "Transform.h"

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
			State = ButtonState::OnMouseEnter;
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
	bool Result = true;

	POINT MousePosition = InputManager::GetInstance().GetMousePosition();
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();
	UMath::Vector2 Position = TransformComp->GetPosition();
	UMath::Vector2 Scale = TransformComp->GetScale();

	assert(Scale.x > 0.0f && Scale.y > 0.0f);

	if (Position.x - Scale.x / 2.f >= MousePosition.x || Position.x + Scale.x / 2.f <= MousePosition.x) Result = false;
	if (Position.y - Scale.y / 2.f >= MousePosition.y || Position.y + Scale.y / 2.f <= MousePosition.y) Result = false;

	return Result;
}