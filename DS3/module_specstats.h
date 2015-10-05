#pragma once
#include "global.h"

class SSModule : Module
{
private:

public:
	virtual void Init();
	virtual void Tick(int time);

	//==========================================
	double integ;

	double integral[2];
	int WidestSpecTimestamp;
	int WidestSpecWidth;
};
