#include "Renderer.h"
#include "../Component/Transform.h"
#include "../Object/Object.h"

void Renderer::Render(HDC dc)
{
	Transform* TransformComp = GetOwner()->GetComponent<Transform>();

	if (!TransformComp) return;

	UMath::Vector2 Position = TransformComp->GetPosition();
	UMath::Vector2 Scale = TransformComp->GetScale();

	if (!Source || Scale.x <= 0.0f || Scale.y <= 0.0f)
		return;

	Gdiplus::Graphics Graphics(dc);

	Graphics.SetInterpolationMode(
		Gdiplus::InterpolationModeHighQualityBicubic
	);

	Graphics.DrawImage(
		Source->GetSource(),
		(INT)(Position.x - Scale.x / 2.f),
		(INT)(Position.y - Scale.y / 2.f),
		(INT)Scale.x,
		(INT)Scale.y
	);
}