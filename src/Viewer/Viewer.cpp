#include "Viewer.h"

void Viewer::Begin()
{
	/*for (auto object : Objects)
	{
		object.Begin();
	}*/
	Objects.push_back(new Object);
}

void Viewer::Update()
{
	for (auto object : Objects)
	{
		if(object)
			object->Update();
	}
}

void Viewer::Render(HDC dc)
{
	for (auto object : Objects)
	{
		if(object)
			object->Render(dc);
	}
}

void Viewer::End()
{
	for (auto object : Objects)
	{
		if(object)
			object->End();
	}
}