#include "Object.h"

void Object::Begin()
{
	for (uint32_t i = 0;i < (uint32_t)ComponentType::End;++i)
	{
		if (Comps[i])
		{
			Comps[i]->Begin();
		}
	}
}

void Object::Update()
{
	for (uint32_t i = 0;i < (uint32_t)ComponentType::End;++i)
	{
		if (Comps[i])
		{
			Comps[i]->Update();
		}
	}
}

void Object::Render()
{
	for (uint32_t i = 0;i < (uint32_t)ComponentType::End;++i)
	{
		if (Comps[i])
		{
			Comps[i]->Render();
		}
	}
}

void Object::End()
{
	for (uint32_t i = 0;i < (uint32_t)ComponentType::End;++i)
	{
		if (Comps[i])
		{
			Comps[i]->End();
		}
	}
}
