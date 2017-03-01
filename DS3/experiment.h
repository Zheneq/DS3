#pragma once
#include "global.h"
#include "medium.h"
// Modules
#include "module_main.h"
#include "module_nrg.h"
#include "module_refl_trans.h"
#include "module_specstats.h"
#include "module_cacher.h"
#include "module_observer.h"
#include "module_inv.h"

class Experiment
{
	Medium *medium;
	std::list<Module*> modules;
	FILE *logFile;
	ObsModule *observer;

	char path[512];
	int expCount;

public:
	Experiment() : medium(nullptr), observer(nullptr), expCount(-1) {}
	~Experiment() { UnLoad(); }
	void Log(const char *msg, bool bToConsole = false);
	void Load(const char *baseinifile, const char *overrideinifile = nullptr);
	void UnLoad();
	void Run();
};