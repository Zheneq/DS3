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
		delete RH;
	}
	for (auto RS : Records)
	{
		delete RS;
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
		Records.push_back(RC->data);

		for (int i = 0; i < RC->get_len(); ++i)
		{
			fprintf(f, "%e, %e\n", realte(i), RC->data[i]);
		}
		fprintf(f, "\n\n");

		RC->data = NULL;
	}
	fclose(f);
}