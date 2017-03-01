#pragma once
#include "global.h"

class CacheModule : public Module
{
public:
	void Init() override;
	void PostCalc(int time) override;
};