#pragma once
#include <concepts>
#include <Windows.h>

class Object;

enum class ComponentType
{
	None,
	Button,
	Transform,
	ListBox,
	TextBox,
	ListView,
	BarChart,
	ChartGroup,
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
	virtual void Begin(HWND WindowHandle) = 0;
	virtual void Update() = 0;
	virtual void Render(HDC dc) {};
	virtual void End() = 0;

	void SetOwner(Object* owner) { Owner = owner; }
	Object* GetOwner() const { return Owner;}

private:
	Object* Owner;
};

template<typename T>
concept ValidComponent = std::derived_from<T, Component> && HasStaticCompType<T>;

template<ValidComponent T>
ComponentType GetCompType()
{
	return T::GetComponentType();
}