#pragma once
#include "Component.h"

enum class ButtonState
{
	None,
	OnMouseDown,
	OnMouseDrag,
	OnMouseEnter,
	OnMouseExit,
	OnMouseOver,
	OnMouseUp,
};

class Button : public Component
{
public:
	virtual void Begin() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void End() override;

	ButtonState GetButtonState() const { return State; }

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::Button;
	}

private:
	bool Interactive();
	ButtonState State;
};