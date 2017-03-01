#pragma once
#include "global.h"


class InvModule : public Module
{
private:

	double *inv[3];

public:
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
};