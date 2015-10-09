#include "module_observer.h"

void ObsModule::Init()
{
	//for (auto rc = RecHeads.begin(); rc < RecHeads.end(); rc++)
	//{
	//	(*rc)->Init();
	//}
}

void ObsModule::AddObserver(int x)
{
	RecHeads.push_back(new RecHead(x, info->nt));
	RecHeads.back()->Init();
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
		RC->data->Dump(fn);
	}

	delete[] fn;
}