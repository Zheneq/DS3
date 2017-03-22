
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <map>
#include <ctime>
#include <thread>
#include "global.h"
#include "experiment.h"
#include "configurer.h"

using namespace std;
default_random_engine *gen = NULL;
double eps = 1e-12; // Точность
char DumpPattern[64] = "%.12e %.12e\n";

/// THIS FUNCTION CAN ONLY BE CALLED FROM MAIN THREAD
map<int, fftw_plan> plans;
fftw_plan& CreatePlan(int n)
{
	plans[n] = fftw_plan_dft_r2c_1d(n, nullptr, nullptr, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
	return plans[n];
}

fftw_plan& GetPlan(int n)
{
	if (plans.count(n))
	{
		return plans[n];
	}
	else
	{
		throw "Plan does not exist.";
	}
}

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
		if (argc > 2)
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
		Configurer config(baseinifile, overrideinifile);

		// Output folders
		strcpy_s(folder, overrideinifile ? overrideinifile : baseinifile);
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


		int expCount = config.GetInteger("ExperimentCount", -1);

		vector<Experiment*> exps;

		for (int i = 0; i < expCount; ++i)
		{
			exps.push_back(new Experiment(i));
			exps.back()->Load(baseinifile, overrideinifile != baseinifile ? overrideinifile : nullptr);
		}

		Medium tempMedium(nullptr);
		tempMedium.Load(config);
		tempMedium.Init();
		CreatePlan(tempMedium.nz);
		CreatePlan(tempMedium.nt);
		
		//*
		vector<thread*> threads;
		/// ~TODO Распарралелить
		for (unsigned int i = 0; i < exps.size(); ++i)
		{
			threads.push_back(new thread(RunExperiment, exps[i]));
		}
		/// ~TODO Синхронизировать
		for (auto& t: threads)
		{
			t->join();
			delete t;
		}
		threads.clear();
		//*/

		/*
		for (unsigned int i = 0; i < exps.size(); ++i)
		{
			RunExperiment(exps[i]);
		}
		//*/




		vector<Module*> modules;
		for (unsigned int i = 0; i < exps[0]->modules.size(); i++)
		{
			modules.clear();

			for (const auto e : exps)
			{
				modules.push_back(e->modules[i]);
			}

			exps[0]->modules[i]->Average(modules);
		}

		for (const auto e : exps)
		{
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

	static void RunExperiment(Experiment* experiment)
	{
		experiment->Run();
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

