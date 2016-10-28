#include "module_observer.h"

void ObsModule::Init()
{
	for (auto rc : RecHeads)
		rc->Init();
	Tick(0);
}

void ObsModule::AddObserver(int x)
{
	RecHeads.push_back(new RecHead(x, info->nt));
	//RecHeads.back()->Init();

	char *msg = new char[256];
	sprintf(msg, "Observer %02d at %f", RecHeads.size() - 1, realxe(x));
	Log(msg);
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
		RC->data->data[time] = info->e->data[RC->idx];
	}
}

void ObsModule::PostCalc(int time)
{
	// Dump all records into single file and reset recheads
	FILE *f = GetFile("records");
	for (auto& RC : RecHeads)
	{
		RC->Records.push_back(RC->data);

		for (int i = 0; i < RC->get_len(); ++i)
		{
			fprintf(f, "%e, %e\n", realte(i), RC->data->data[i]);
		}
		fprintf(f, "\n\n");

		RC->data = NULL;
	}
	fclose(f);


	Average();

	printf("\n ");
	for (auto& RC : RecHeads)
	{
		delete RC->data;
		RC->data = nullptr;
		printf("I have %d records. \n", RC->Records.size());
	}
}

void ObsModule::Average()
{
	int idx = 0;
	char *fn = new char[64];
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

		sprintf(fn, "rec%03d", idx);
		FILE *f = GetFile(fn);
		sprintf(fn, "rec%03d-spec", idx);
		FILE *fs = GetFile(fn);

		RC->data->DumpFullPrecision(f, fs, realte, realspect);
		//for (auto rec : RC->Records)
		//{
		//	rec->DumpFullPrecision(f, fs, realte, realspect);
		//}

		fclose(f);
		fclose(fs);

		idx++;
	}

	delete[] fn;
}