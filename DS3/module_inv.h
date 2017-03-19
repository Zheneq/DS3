#pragma once
#include "module.h"


class InvModule : public Module
{
private:

	double *inv[5];
	FILE *f[5];
	double *h_cache;

public:
	explicit InvModule(Experiment* e = nullptr) : Module(e) {}
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
};