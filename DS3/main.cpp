
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <ctime>
#include "global.h"
#include "experiment.h"

using namespace std;
default_random_engine *gen = NULL;
double eps = 1e-12; // Точность
char DumpPattern[64] = "%.12 %.12";

class RandomExperiment
{
	char *baseinifile, *overrideinifile;
	char folder[512];

public:
	void Init(int argc, char **argv)
	{
		if (argc < 2)
		{
			throw "Too few command-line arguments";
		}
		baseinifile = argv[1];
		if (argc < 3)
		{
			overrideinifile = argv[2];
		}
		else
		{
			overrideinifile = baseinifile;
		}

	}

	void Run()
	{
		INIReader baseconfig(baseinifile), overconfig(overrideinifile);
		if (baseconfig.ParseError() < 0 || overconfig.ParseError() < 0) {
			throw("Can't load ini file");
		}

		// Output folders
		strcpy_s(folder, overrideinifile);
		for (int i = strlen(folder); i; --i) {
			if (folder[i] == '.')
			{
				folder[i] = '\0';
				break;
			}
		}

		_mkdir(folder);

		char path[512];
		sprintf_s(path, "%s/Average", folder);
		_mkdir(path);



		// char DumpPatternPattern[] = "%%.%df, %%.%df\n";
		// int dig = (int)ceil(log(1 / (hs*hs)) / log(10)) + 4;
		// eps = pow(.1, dig) / 2 - 1e-10;
		// sprintf(DumpPattern, DumpPatternPattern, dig, dig);


		int expCount = baseconfig.GetInteger("Data", "ExperimentCount", -1);
		expCount = overconfig.GetInteger("Data", "ExperimentCount", expCount);

		vector<Experiment*> exps;

		for (int i = 0; i < expCount; ++i)
		{
			exps.push_back(new Experiment(i));
			exps.back()->Load(baseinifile, overrideinifile != baseinifile ? overrideinifile : nullptr);
		}
		
		/// TODO Распарралелить
		for (const auto e : exps)
		{
			e->Run();
		}
		/// TODO Синхронизировать

		vector<Module*> modules;
		for (unsigned int i = 0; i < exps[0]->modules.size(); i++)
		{
			modules.clear();

			for (const auto e : exps)
			{
				modules.push_back(e->modules[i]);
			}

			exps[0]->modules[i]->Collect(modules);
		}

		for (const auto e : exps)
		{
			for (const auto m : e->modules)
			{
				m->Average();
			}
		}

		for (const auto e : exps)
		{
			e->UnLoad();
			delete e;
		}
		exps.clear();
	}

	FILE *GetFileAverage(const char *name)
	{
		char DumpPath[512];
		sprintf_s(DumpPath, "%s/Average/%s.txt", folder, name);
		FILE *f = nullptr;
		fopen_s(&f, DumpPath, "wt");

		if (!f)
		{
			char Error[544];
			sprintf_s(Error, "Cannot create file: %s", DumpPath);
			throw(Error);
		}

		return f;
	}
};


int main(int argc, char **argv)
{
	gen = new default_random_engine((unsigned int)time(0));

	RandomExperiment e;
	e.Init(argc, argv);
	e.Run();

	return 0;
}

