#include "experiment.h"
#include <ctime>
#include <iostream>

void Experiment::Log(const char *msg, bool bToConsole)
{
	if (!logFile) throw("No log file!");

	fprintf(logFile, "%s\n", msg);
	printf("\n\tLog:\t%s\n", msg);
}

void Experiment::Load(const char *baseinifile, const char *overrideinifile)
{
	INIReader baseconfig(baseinifile), overconfig(overrideinifile);

	if (baseconfig.ParseError() < 0 || overrideinifile && overconfig.ParseError() < 0) {
		throw("Can't load ini file");
	}

	medium = new Medium();
	medium->Load(&baseconfig);
	medium->Load(&overconfig);

	// Output folders
	strcpy_s(path, overrideinifile ? overrideinifile : baseinifile);
	for (int i = strlen(path); i; --i) {
		if (path[i] == '.')
		{
			path[i] = '\0';
			break;
		}
	}

	_mkdir(path);

	char Folder[512];
	for (int i = 0; i <= expCount; ++i)
	{
		sprintf(Folder, "%s/E%03d", path, i);
		_mkdir(Folder);
	}
	sprintf(Folder, "%s/Average", path);
	_mkdir(Folder);

	// Init logger
	char *fn = new char[256];
	sprintf(fn, "%s/log.txt", path);
	logFile = fopen(fn, "w");
	delete[] fn;

	// Модули
	observer = new ObsModule;

	modules.push_back(new MainModule);
	modules.push_back(new InvModule);
	modules.push_back(new SSModule);
	modules.push_back(observer);
}

void Experiment::UnLoad()
{
	fclose(logFile);

	if (medium) delete medium;
	for (auto m : modules) delete m;
	modules.clear();
}

void Experiment::Run()
{
	try
	{
		medium->Init();

		observer->AddObserver(medium->idxxe(-50), "refl");
		observer->AddObserver(medium->idxxe(145), "trans");

		for (auto m : modules)
			m->Init();

		///////////////////////////////

		char *m = new char[128];
		Log("Structure");
		for (int i = 0; i < medium->LayerCount; ++i)
		{
			sprintf(m, DumpPattern, medium->Layers[i].right - medium->Layers[i].left, medium->Layers[i].dc);
			Log(m);
		}
		Log("Structure end");

		field* dp = new field;
		dp->Init(medium->nz, FFTW_ESTIMATE);
		for (int i = 0; i < dp->GetLen(); ++i)
		{
			dp->data[i] = medium->DielCond(i);
		}

		FILE *f = GetFile("DielCond");
		dp->DumpFullPrecision(f, NULL);
		fclose(f);


		field* ab = new field;
		ab->Init(medium->nz, FFTW_ESTIMATE);
		for (int i = 0; i < ab->GetLen(); ++i)
		{
			ab->data[i] = medium->Absorption(i);
		}

		f = GetFile("Absorption");
		ab->DumpFullPrecision(f, NULL);
		fclose(f);

		///////////////////////////////

		// Просчёт
		int time;
		for (time = 1; time < medium->nt; ++time)
		{
			printf("\r%02d of %d: %d/%d          ", ExperimentNum, ExperimentCount, time, medium->nt - 1);

			for (auto m : modules)
				m->Tick(time);
		}
		printf("\n");

		for (auto m : modules)
			m->PostCalc(time);
	}
	catch (char *error)
	{
		cerr << error << endl;
		UnLoad();
		return;
	}
	UnLoad();
}

FILE *GetFile(const char *name)
{
	char DumpPath[512];
	if (expNum < expCount)
		sprintf(DumpPath, "%s/E%03d/%s.txt", config->Get("Data", "DumpPath", "").c_str(), expNum, name);
	else
		sprintf(DumpPath, "%s/Average/%s.txt", config->Get("Data", "DumpPath", "").c_str(), name);
	FILE *f = fopen(DumpPath, "wt");

	if (!f)
	{
		char Error[544];
		sprintf(Error, "Cannot create file: %s", DumpPath);
		throw(Error);
	}

	return f;
}