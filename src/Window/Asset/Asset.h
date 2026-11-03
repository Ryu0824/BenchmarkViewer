#pragma once

class Asset
{
public:
	virtual void Load() = 0;
	virtual ~Asset() = default;
};