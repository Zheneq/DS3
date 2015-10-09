
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include "global.h"

// Modules
#include "module_main.h"
#include "module_nrg.h"
#include "module_refl_trans.h"
#include "module_specstats.h"
#include "module_cacher.h"

using namespace std;

std::list<Module*> modules;
field *e_cache = NULL, *h_cache = NULL;

int ExperimentNum = -1, ExperimentCount = -1;

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

	// Модули

	// for (auto m : modules)
	//	delete m;
	// modules.clear();

	modules.push_back(new MainModule);
	modules.push_back(new NRGModule);
	modules.push_back(new RTModule);
	modules.push_back(new SSModule);
	modules.push_back(new CacheModule);
}

void Init(int argc, char **argv)
{
	if (info) delete info;
	info = new EnvInfo();

	//
	//	RecHeads.push_back(new RecHead(idxxe(obj_raw.left - 10), nt));
	//	RecHeads.push_back(new RecHead(idxxe(obj_raw.right + 10), nt));

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
		Load(argc, argv);

		ExperimentCount = config->GetInteger("Data", "ExperimentCount", -1);
		for (ExperimentNum = 0; ExperimentNum < ExperimentCount; ++ExperimentNum)
		{
			Init(argc, argv);
			for (auto m : modules)
				m->Init();

			// Просчёт
			int time;
			for (time = 1; time < info->nt; ++time)
			{
				printf("\r%d/%d          ", time, info->nt - 1);

				for (auto m : modules)
					m->Tick(time);
			}
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
		return 1;
	}

	return 0;
}