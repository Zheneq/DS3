#pragma once
#include "global.h"

class SSModule : public Module
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
