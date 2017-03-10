#pragma once
#include "module.h"

class MainModule : public Module
{
public:
	explicit MainModule(Experiment* e = nullptr) : Module(e) {}
	virtual void Init() override;
	virtual void Tick(int time) override;
	void PostCalc(int time) override;
};
