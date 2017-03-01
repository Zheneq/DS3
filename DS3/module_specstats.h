#pragma once
#include "global.h"

class SSModule : public Module
{
private:

public:
	virtual void Init() override;
	virtual void Tick(int time) override;

	//==========================================
	double integ;

	double integral[2];
	int WidestSpecTimestamp;
	int WidestSpecWidth;
};
