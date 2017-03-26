#include "experiment.h"
#include <ctime>
#include <iostream>
#include "configurer.h"

void Experiment::Log(const char *msg, bool bToConsole)
{
	if (!logFile) throw("No log file!");

	fprintf(logFile, "%s\n", msg);
	if (bToConsole) printf("\n\tLog: [%02d]\t%s\n", id, msg);
}

void Experiment::Load(const char *baseinifile, const char *overrideinifile)
{
	Configurer config(baseinifile, overrideinifile);

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

	sprintf(path_avg, "%s/Average", path);
	_mkdir(path_avg);

	sprintf(path, "%s/E%03d", path, id);
	_mkdir(path);

	// Init logger
	char *fn = new char[256];
	sprintf(fn, "%s/log.txt", path);
	logFile = fopen(fn, "w");
	delete[] fn;

	medium = new Medium(this);
	medium->Load(config);

	// Модули
	observer = new ObsModule(this);

	modules.push_back(new MainModule(this));
	modules.push_back(observer);
	modules.push_back(new InvModule(this));
}

Experiment::~Experiment()
{
	if (logFile) fclose(logFile);

	if (medium) delete medium;
	for (auto m : modules) delete m;
	modules.clear();
}

void Experiment::Run()
{
	try
	{
		medium->Init();

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
		delete[] m;

		{
			field* dp = new field(medium->nz, FFTW_ESTIMATE);
			for (int i = 0; i < dp->GetLen(); ++i)
			{
				dp->data[i] = medium->DielCond(i);
			}

			FILE *f = GetFile("DielCond");
			dp->DumpFullPrecision(f, NULL, medium);
			fclose(f);
			delete dp;

		}

		{
			field* ab = new field(medium->nz, FFTW_ESTIMATE);
			for (int i = 0; i < ab->GetLen(); ++i)
			{
				ab->data[i] = medium->Absorption(i);
			}

			FILE *f = GetFile("Absorption");
			ab->DumpFullPrecision(f, NULL, medium);
			fclose(f);
			delete ab;
		}

		///////////////////////////////

		// Просчёт
		int time;
		for (time = 1; time < medium->nt; ++time)
		{
			printf("\r%02d of ??: %d/%d          ", id, time, medium->nt - 1);

			for (auto m : modules)
				m->Tick(time);
		}
		printf("\n");

		for (auto m : modules)
			m->PostCalc(time);

		bHasFinished = true;
	}
	catch (char *error)
	{
		cerr << error << endl;
		return;
	}
}

FILE *Experiment::GetFile(const char *name)
{
	char DumpPath[512];
	sprintf_s(DumpPath, "%s/%s.txt", bHasFinished ? path_avg : path, name);
	FILE *f = nullptr;
	fopen_s(&f, DumpPath, "wt");

	if (!f)
	{
		char Error[544];
		sprintf(Error, "Cannot create file: %s", DumpPath);
		throw(Error);
	}

	return f;
}