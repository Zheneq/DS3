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
		RC->Records.push_back(RC->data);

		for (int i = 0; i < RC->get_len(); ++i)
		{
			fprintf(f, "%e, %e\n", experiment->medium->realte(i), RC->data->data[i]);
		}
		fprintf(f, "\n\n");

		RC->data = NULL;
	}
	fclose(f);
}

void ObsModule::Average()
{
	int idx = 0;
	char fn[64];
	for (auto& RC : RecHeads)
	{
		RC->Init();

		for (int i = 0; i < RC->get_len(); ++i)
		{
			double sum = 0;
			for (auto rec : RC->Records)
			{
				sum += rec->data[i];
			}
			RC->data->data[i] = sum / RC->Records.size();
		}

		RC->data->Fourier();

		sprintf_s(fn, "rec%03d", idx);
		FILE *f = experiment->GetFile(fn);
		sprintf_s(fn, "rec%03d-spec", idx);
		FILE *fs = experiment->GetFile(fn);

		RC->data->DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);

		fclose(f);
		fclose(fs);

		idx++;
	}
}