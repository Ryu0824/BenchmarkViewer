#include "Button.h"
#include "../Manager/InputManager.h"
#include "Transform.h"
#include "../Object/Object.h"

void Button::Begin()
{

}

void Button::Update()
{
	if (Interactive())
	{
		if (State == ButtonState::None || State == ButtonState::OnMouseExit)
		{
			State = ButtonState::OnMouseEnter;
#ifdef _DEBUG
			printf("ButtonComponent : Enter Event Occured\n");
#endif 
		}
		else if (State == ButtonState::OnMouseEnter)
		{
			State = ButtonState::OnMouseOver;
		}
		else if (State == ButtonState::OnMouseOver)
		{
			// Click
			if (InputManager::GetInstance().IsPressed(VK_LBUTTON))
			{
#ifdef _DEBUG
				printf("ButtonComponent : Click Event Occured\n");
#endif
			}
		}
	}
	else
	{
		if (State == ButtonState::None || State == ButtonState::OnMouseEnter || State == ButtonState::OnMouseOver || State == ButtonState::OnMouseUp)
		{
			State = ButtonState::OnMouseExit;
#ifdef _DEBUG
			printf("ButtonComponent : Exit Event Occured\n");
#endif 
		}
	}
}

void Button::Render(HDC dc)
{
	Component::Render(dc);

	Transform* TransformComp = GetOwner()->GetComponent<Transform>();
	UMath::Vector2 Position = TransformComp->GetPosition();
	UMath::Vector2 Scale = TransformComp->GetScale();

	Rectangle(dc,
		int(Position.x - Scale.x / 2.f),
		int(Position.y - Scale.y / 2.f),
		int(Position.x + Scale.x / 2.f),
		int(Position.y + Scale.y / 2.f));
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