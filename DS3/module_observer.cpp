#include "module_observer.h"
#include "experiment.h"

// const auto info = experiment->medium;
void ObsModule::Init()
{
	for (auto rc : RecHeads)
		rc->Init();
	Tick(0);
}

void ObsModule::AddObserver(int x, const char* name)
{
	RecHeads.push_back(new RecHead(x, experiment->medium->nt));
	RecHeadNames.push_back(string(name));
	//RecHeads.back()->Init();

	char *msg = new char[256];
	sprintf_s(msg, 256, "Observer %02d (%s) at %f", RecHeads.size() - 1, RecHeadNames.back().c_str(), experiment->medium->realxe(x));
	experiment->Log(msg);
	delete[] msg;
}

ObsModule::~ObsModule()
{
	for (auto RH : RecHeads)
	{
		if (RH) delete RH;
	}
}

void ObsModule::Tick(int time)
{
	for (auto& RC : RecHeads)
	{
		RC->data->data[time] = experiment->medium->e->data[RC->idx];
	}
}

void ObsModule::PostCalc(int time)
{
	// Dump all records into single file
	FILE *f = experiment->GetFile("records");
	for (auto& RC : RecHeads)
	{
		// RC->Records.push_back(RC->data);

		for (int i = 0; i < RC->get_len(); ++i)
		{
			fprintf(f, "%e, %e\n", experiment->medium->realte(i), RC->data->data[i]);
		}
		fprintf(f, "\n\n");

		// RC->data = NULL;
	}
	fclose(f);
}

void ObsModule::Average(vector<Module*> modules)
{
	char fn[64];

	field *avrg = new field();
	avrg->Init(experiment->medium->nt);

	for (unsigned int i = 0; i < RecHeads.size(); ++i) // Observers
	{
		for (int j = 0; j < experiment->medium->nt; ++j) // Time slices
		{
			double sum = 0;
			for (unsigned int k = 0; k < modules.size(); ++k) // Realizations
			{
				sum += ((ObsModule*)modules[k])->RecHeads[i]->data->data[j];
			}
			avrg->data[j] = sum / modules.size();
		}

		avrg->Fourier();

		sprintf_s(fn, "rec%03d", i);
		FILE *f = experiment->GetFile(fn);
		sprintf_s(fn, "rec%03d-spec", i);
		FILE *fs = experiment->GetFile(fn);

		avrg->DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);

		for (const auto &m : modules)
		{
			((ObsModule*)m)->RecHeads[i]->data->DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);
		}

		fclose(f);
		fclose(fs);
	}
}