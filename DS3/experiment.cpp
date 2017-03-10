#include "experiment.h"
#include <ctime>
#include <iostream>

void Experiment::Log(const char *msg, bool bToConsole)
{
	if (!logFile) throw("No log file!");

	fprintf(logFile, "%s\n", msg);
	printf("\n\tLog: [%02d]\t%s\n", id, msg);
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
	sprintf(Folder, "%s/Average", path);
	_mkdir(Folder);

	sprintf(path, "%s/E%03d", path, id);
	_mkdir(path);

	// Init logger
	char *fn = new char[256];
	sprintf(fn, "%s/log.txt", path);
	logFile = fopen(fn, "w");
	delete[] fn;

	// Модули
	observer = new ObsModule(this);

	modules.push_back(new MainModule(this));
	modules.push_back(new InvModule(this));
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
		dp->DumpFullPrecision(f, NULL, medium);
		fclose(f);


		field* ab = new field;
		ab->Init(medium->nz, FFTW_ESTIMATE);
		for (int i = 0; i < ab->GetLen(); ++i)
		{
			ab->data[i] = medium->Absorption(i);
		}

		f = GetFile("Absorption");
		ab->DumpFullPrecision(f, NULL, medium);
		fclose(f);

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
	}
	catch (char *error)
	{
		cerr << error << endl;
		UnLoad();
		return;
	}
	UnLoad();
}

FILE *Experiment::GetFile(const char *name)
{
	char DumpPath[512];
	sprintf_s(DumpPath, "%s/E%03d/%s.txt", path, id, name);
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