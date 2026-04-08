#pragma once
class IStart
{
public:
	virtual ~ IStart() {}
	virtual void Init() = 0;
};