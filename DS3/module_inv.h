#pragma once
#include "module.h"


class InvModule : public Module
{
private:

	double *inv[3];

public:
	explicit InvModule(Experiment* e = nullptr) : Module(e) {}
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
};