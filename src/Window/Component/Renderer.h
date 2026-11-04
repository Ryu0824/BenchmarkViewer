#pragma once
#include "Component.h"
#include "../Asset/Texture2D.h"

class Renderer : public Component
{
public:
	virtual void Begin(HWND WindowHandle) override {};
	virtual void Update() override {};
	virtual void Render(HDC dc) override;
	virtual void End() override {};

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::Renderer;
	}

	void SetTexture(Texture2D* InSource) { Source = InSource; }

private:
	Texture2D* Source;
};