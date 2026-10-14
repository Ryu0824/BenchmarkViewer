#include "Viewer.h"
#include "../Window/Component/Button.h"
#include "../Window/Component/Transform.h"

void Viewer::Begin()
{
	/*for (auto object : Objects)
	{
		object.Begin();
	}*/
	Object* testObject = new Object;
	Transform* TransformComp = testObject->AddComponent<Transform>();
	testObject->AddComponent<Button>();

	TransformComp->SetPosition(UMath::Vector2(640.f, 360.f));
	TransformComp->SetScale(UMath::Vector2(200.f, 200.f));

	Objects.push_back(testObject);


	// 4 Object
	// BenchMark Resource Object

	// BenchMark Resource -> Compare Standard

	// BenchMark Enviroment

	// BenchMark Chart
}

void Viewer::Update()
{
	for (auto object : Objects)
	{
		if (object)
			object->Update();
	}
}

void Viewer::Render(HDC dc)
{
	for (auto object : Objects)
	{
		if (object)
			object->Render(dc);
	}
}

void Viewer::End()
{
	for (auto object : Objects)
	{
		if (object)
			object->End();
	}
}