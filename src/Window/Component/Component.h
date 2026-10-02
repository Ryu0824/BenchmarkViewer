#pragma once
#include <concepts>

enum class ComponentType
{
	None,
	Button,
	End,
};

template<typename T>
concept HasStaticCompType = requires
{
	{T::GetComponentType()}->std::same_as<ComponentType>;
};

class Component
{
public:
	virtual void Begin() = 0;
	virtual void Update() = 0;
	virtual void Render() {};
	virtual void End() = 0;
};

template<typename T>
concept ValidComponent = std::derived_from<T, Component> && HasStaticCompType<T>;

template<ValidComponent T>
ComponentType GetCompType()
{
	return T::GetComponentType();
}