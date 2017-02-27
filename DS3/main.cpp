
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <ctime>
#include "global.h"

// Modules
#include "module_main.h"
#include "module_nrg.h"
#include "module_refl_trans.h"
#include "module_specstats.h"
#include "module_cacher.h"
#include "module_observer.h"
#include "module_inv.h"

using namespace std;

FILE *LogFile = NULL;
std::list<Module*> modules;
field *e_cache = NULL, *h_cache = NULL;
ObsModule *obs = NULL;
default_random_engine *gen = NULL;

int ExperimentNum = -1, ExperimentCount = -1;

void Log(const char *msg, bool bToConsole)
{
	if (!LogFile) throw("No log file!");

	fprintf(LogFile, "%s\n", msg);
	printf("\n\tLog:\t%s\n", msg);
}

void Load(int argc, char **argv)
{
	const unsigned len = 512;
	char Path[len];

	// Если не задан файл конфигурации, ищем дефолтный
	if (argc == 1)
	{
		strcpy(Path, argv[0]); //strcpy_s(Path, strlen(argv[0]), argv[0]);
		for (int i = strlen(argv[0]); i; --i) if (Path[i] == '\\' || Path[i] == '/') { Path[i] = '\0'; break; }
		strcat(Path, "\\default.ini");
	}
	else
	{
		strcpy(Path, argv[1]);
	}
	printf("%s\n", Path);

	if (config) delete config;
	config = new INIReader(Path);

	if (config->ParseError() < 0) {
		throw("Can't load ini file");
	}
	
	//std::cout << "Config loaded from 'test.ini': version="
	//	<< reader.GetInteger("protocol", "version", -1) << ", name="
	//	<< reader.Get("user", "name", "UNKNOWN") << ", email="
	//	<< reader.Get("user", "email", "UNKNOWN") << ", pi="
	//	<< reader.GetReal("user", "pi", -1) << ", active="
	//	<< reader.GetBoolean("user", "active", true) << "\n";

	// As we have config now, we can create a temprary info object
	info = new EnvInfo();

	// Output folders
	_mkdir(config->Get("Data", "DumpPath", "").c_str());

	char Folder[512];
	int ExpCount = config->GetInteger("Data", "ExperimentCount", -1);
	for (int i = 0; i <= ExpCount; ++i)
	{
		sprintf(Folder, "%s/E%03d", config->Get("Data", "DumpPath", "").c_str(), i);
		_mkdir(Folder);
	}
	sprintf(Folder, "%s/Average%03d", config->Get("Data", "DumpPath", "").c_str(), ExpCount);
	_mkdir(Folder);

	// Init logger
	char *fn = new char[256];
	sprintf(fn, "%s/log.txt", config->Get("Data", "DumpPath", "").c_str());
	LogFile = fopen(fn, "w");
	delete[] fn;

	// Модули

	// for (auto m : modules)
	//	delete m;
	// modules.clear();

	obs = new ObsModule;

	obs->AddObserver(idxxe(-50), "refl");
	//obs->AddObserver(idxxe(-250));
	//obs->AddObserver(idxxe(-500));
	obs->AddObserver(idxxe(145), "trans");
	//obs->AddObserver(idxxe(530));
	//obs->AddObserver(idxxe(780));

	modules.push_back(new MainModule);
//	modules.push_back(new NRGModule);
	modules.push_back(new InvModule);
//	modules.push_back(new RTModule);
	modules.push_back(new SSModule);
	modules.push_back(obs);

}

void UnLoad()
{
	fclose(LogFile);
}

void Init(int argc, char **argv)
{
	if (info) delete info;
	info = new EnvInfo();

	// Начальные значения
	double xe = realxe(0);
	for (int i = 0; i<info->nz; xe += info->hs, ++i)
	{
		info->h->data[i] = info->e->data[i] = exp(-((xe*xe) / (info->a*info->a)))*cos(info->cf*xe);
	}


}

int main(int argc, char **argv)
{
	try
	{
		gen = new default_random_engine((unsigned int)time(0));

		Load(argc, argv);

		ExperimentCount = config->GetInteger("Data", "ExperimentCount", -1);
		for (ExperimentNum = 0; ExperimentNum < ExperimentCount; ++ExperimentNum)
		{
			Init(argc, argv);
			for (auto m : modules)
				m->Init();

			///////////////////////////////

			char *m = new char[128];
			Log("Structure");
			for (int i = 0; i < info->LayerCount; ++i)
			{
				sprintf(m, info->DumpPattern, info->Layers[i].right - info->Layers[i].left, info->Layers[i].dc);
				Log(m);
			}
			Log("Structure end");

			field* dp = new field;
			dp->Init(info->nz, FFTW_ESTIMATE);
			for (int i = 0; i < dp->GetLen(); ++i)
			{
				dp->data[i] = DielCond(i);
			}

			FILE *f = GetFile("DielCond");
			dp->DumpFullPrecision(f, NULL);
			fclose(f);


			field* ab = new field;
			ab->Init(info->nz, FFTW_ESTIMATE);
			for (int i = 0; i < ab->GetLen(); ++i)
			{
				ab->data[i] = Absorption(i);
			}

			f = GetFile("Absorption");
			ab->DumpFullPrecision(f, NULL);
			fclose(f);

			///////////////////////////////

			// Просчёт
			int time;
			for (time = 1; time < info->nt; ++time)
			{
				printf("\r%02d of %d: %d/%d          ", ExperimentNum, ExperimentCount, time, info->nt - 1);

				for (auto m : modules)
					m->Tick(time);
			}
			printf("\n");

			for (auto m : modules)
				m->PostCalc(time);
		}

		for (auto m : modules)
			m->Average();
/*
		{
			Init(argc, argv);
			for (auto m : modules)
				m->Init();

			// "Просчёт"
			memcpy(info->e->data, info->e_cache->data, sizeof(info->e_cache->data[0]) * info->e->GetLen());
			memcpy(info->h->data, info->h_cache->data, sizeof(info->h_cache->data[0]) * info->h->GetLen());

			for (auto m : modules)
					m->Tick(-1);
			
			for (auto m : modules)
				m->PostCalc(-1);
		}
*/
	}
	catch (char *error)
	{
		cerr << error << endl;
		UnLoad();
		return 1;
	}
	UnLoad();

	return 0;
}