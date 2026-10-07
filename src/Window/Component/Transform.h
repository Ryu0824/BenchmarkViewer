#pragma once

#include "Component.h"
#include <cassert>

namespace UMath
{
	struct Vector2
	{
		Vector2() :x(0.0f), y(0.0f) {};
		Vector2(float inX, float inY) :x(inX), y(inY) {};

		void operator=(const Vector2& Other)
		{
			x = Other.x;
			y = Other.y;
		}

		float x;
		float y;
	};
}

class Transform : public Component
{
public:
	virtual void Begin() override;
	virtual void Update() override;
	virtual void End() override;

	UMath::Vector2 GetPosition() const { return Position; }
	void SetPosition(UMath::Vector2 position) { Position = position; }

	UMath::Vector2 GetScale() const { return Scale; }
	void SetScale(UMath::Vector2 scale) { Scale = scale; }

	UMath::Vector2 GetRotation() const { return Rotation; }
	void SetRotation(UMath::Vector2 rotation) { Rotation = rotation; }

	static constexpr ComponentType GetComponentType()
	{
		return ComponentType::Transform;
	}

private:
	UMath::Vector2 Position;
	UMath::Vector2 Scale;
	UMath::Vector2 Rotation;
};