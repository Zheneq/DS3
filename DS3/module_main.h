#pragma once
#include "global.h"

class MainModule : public Module
{
private:
	double StructureLeftEdge; // Левая граница слоя

	int DumpFrameStep; // Шаг дампа

public:
	virtual void Init() override;
	virtual void Tick(int time) override;
	void PostCalc(int time) override;
};
