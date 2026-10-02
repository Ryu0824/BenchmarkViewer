#pragma once

#include <vector>
#include "../Component/Component.h"

class Object
{
public:
	void Begin() = 0;
	void Update() = 0;
	void Render() {};
	void End() = 0;

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

	void RemoveComponent()
	{

	}

private:
	Component* Comps[(uint32_t)ComponentType::End] = {};
};