#pragma once
#include "global.h"


class InvModule : public Module
{
private:

	double *inv[3];

public:
	virtual void Init();
	virtual void Tick(int time);
	virtual void PostCalc(int time);
};