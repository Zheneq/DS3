#pragma once
#include "global.h"

class CacheModule : public Module
{
public:
	void Init();
	void PostCalc(int time);
};