#pragma once

#include <vector>
#include <Windows.h>
#include "../Component/Component.h"

class Object
{
public:
	void Begin();
	void Update();
	void Render(HDC dc);
	void End();

	template<typename T, typename... Args>
	Component* AddComponent(Args&&... args)
	{
		ComponentType Type = GetCompType<T>();
		static_assert((uint32_t)Type < (uint32_t)ComponentType::End, "This is Not Component!!");

		if (Comps[(uint32_t)Type] != nullptr)
		{
			Comps[(uint32_t)Type] = new T(std::forward<Args>(args)...);
		}

		return Comps[(uint32_t)Type];
	}

	template<typename T>
	void RemoveComponent()
	{
		ComponentType Type = GetCompType<T>();
		Component* comp = Comps[(uint32_t)Type];

		if (comp)
		{
			delete comp;
			Comps[(uint32_t)Type] = nullptr;
		}
	}

private:
	Component* Comps[(uint32_t)ComponentType::End] = {};
};