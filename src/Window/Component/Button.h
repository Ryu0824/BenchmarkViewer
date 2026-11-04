#pragma once
#include "Component.h"
#include "../Manager/EventManager.h"
#include "../Asset/Texture2D.h"

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
	Button() = default;
	~Button() = default;

	virtual void Begin(HWND WindowHandle) override;
	virtual void Update() override;
	virtual void Render(HDC dc) override;
	virtual void End() override;

	ButtonState GetButtonState() const { return State; }

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::Button;
	}

	void SetClickEvent(std::function<void()> func) 
	{
		Callback<void()> CallBackInst;
		CallBackInst.Bind(func);
	
		OnClickEvent.Subscribe(CallBackInst);
	}

	void SetTexture(int Index, Texture2D* InSource)
	{
		ButtonTexture[Index] = InSource;
	}

private:
	bool Interactive();
	ButtonState State;
	Event<void()> OnClickEvent;

	Texture2D* ButtonTexture[3];
};