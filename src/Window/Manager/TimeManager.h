#pragma once

class TimeManaegr
{
public:
	static TimeManaegr& GetInstance()
	{
		static TimeManaegr Instance;
		return Instance;
	}

	void Begin();
	void Update();
	void End();

private:
	float DeltaTime = 0.0f;

	TimeManaegr() = default;
	~TimeManaegr() = default;
};