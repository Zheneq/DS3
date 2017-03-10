#pragma once
#include "global.h"
#include "medium.h"
// Modules
#include "module_main.h"
#include "module_observer.h"
#include "module_inv.h"

class Experiment
{
public:
	Medium *medium;
	std::vector<Module*> modules;
private:
	FILE *logFile;
	ObsModule *observer;

	char path[512];
	int id;

public:
	Experiment(int _id = -1) : medium(nullptr), observer(nullptr), id(_id) {}
	~Experiment() { UnLoad(); }
	Experiment(const Experiment&) = delete;
	void Log(const char *msg, bool bToConsole = false);
	void Load(const char *baseinifile, const char *overrideinifile = nullptr);
	void UnLoad();
	void Run();
	FILE *GetFile(const char *name);
};