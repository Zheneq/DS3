#include "module_observer.h"
#include "experiment.h"

// const auto info = experiment->medium;
void ObsModule::Init()
{
	AddObserver(0, "left");
	AddObserver(experiment->medium->nz - 1, "right");

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

const RecHead& ObsModule::GetObserver(const char *name)
{
	const auto item = string(name);
	for (unsigned int i = 0; i < RecHeadNames.size(); ++i)
	{
		if (RecHeadNames[i] == item)
		{
			return *(RecHeads[i]);
		}
	}

	throw "Observer does not exist.";
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
		RC->e->data[time] = experiment->medium->e->data[RC->idx];
		RC->h->data[time] = experiment->medium->h->data[RC->idx];
	}
}

void ObsModule::PostCalc(int time)
{
	// Dump all records into single file
	// TODO: А оно надо?
	FILE *f = experiment->GetFile("records");
	for (auto& RC : RecHeads)
	{
		RC->e->DumpFullPrecision(f, nullptr, experiment->medium, &Medium::realte, nullptr);
	}
	fclose(f);
}

void ObsModule::Average(vector<Module*> modules)
{
	char fn[64];

	field avrg(experiment->medium->nt);

	for (unsigned int i = 0; i < RecHeads.size(); ++i) // Observers
	{
		for (int j = 0; j < experiment->medium->nt; ++j) // Time slices
		{
			double sum = 0;
			for (unsigned int k = 0; k < modules.size(); ++k) // Realizations
			{
				sum += ((ObsModule*)modules[k])->RecHeads[i]->e->data[j];
			}
			avrg.data[j] = sum / modules.size();
		}

		avrg.Fourier();

		sprintf_s(fn, "rec_%s", RecHeadNames[i].c_str());
		FILE *f = experiment->GetFile(fn);
		sprintf_s(fn, "rec_%s-spec", RecHeadNames[i].c_str());
		FILE *fs = experiment->GetFile(fn);

		avrg.DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);

		for (const auto &m : modules)
		{
			((ObsModule*)m)->RecHeads[i]->e->DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);
		}

		fclose(f);
		fclose(fs);
	}
}