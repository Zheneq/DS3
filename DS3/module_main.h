#pragma once
#include "global.h"

class MainModule : public Module
{
private:
	double StructureLeftEdge; // Левая граница слоя

public:
	virtual void Init();
	virtual void Tick(int time);
};
