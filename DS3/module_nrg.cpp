#include "module_nrg.h"

double nrg_level[] = { .1, .075, .05, .025 };
int nrgpt[ARRAYSIZE(nrg_level)];
int nrgpe[ARRAYSIZE(nrg_level)];

DumpData dmp[] =
{
	{ NULL, "inv1", 0 },
	{ NULL, "inv2", 0 },
	{ NULL, "inv3", 0 },
	{ NULL, "nrg", 0 },
	{ NULL, "nrg1", 0 },
	{ NULL, "nrg2", 0 },
	{ NULL, "nrg3", 0 },
	{ NULL, "nrg1d", 0 },
	{ NULL, "nrg2d", 0 },
	{ NULL, "int", 0 },
	{ NULL, "nrgE", 0 },
	{ NULL, "nrg1E", 0 },
	{ NULL, "nrg2E", 0 },
	{ NULL, "nrg3E", 0 },
	{ NULL, "nrg1dE", 0 },
	{ NULL, "nrg2dE", 0 },
	{ NULL, "nrg1dEts", 0 },
	{ NULL, "nrg2dEts", 0 },
	{ NULL, "nrg3dEts", 0 },
	{ NULL, "nrg2ds", 0 },
	{ NULL, "nrg2des", 0 },
};

void NRGModule::Init()
{
	for (int i = 0; i < ARRAYSIZE(nrg_level); ++i)
	{
		nrgpt[i] = -1;
		nrgpe[i] = -1;
	}
	for (int i = 0; i < ARRAYSIZE(dmp); ++i)
	{
		dmp[i].file = GetFile(dmp[i].name.c_str());
	}
}

void NRGModule::Tick(int time)
{
	// —читаем инварианты
	inv3 = inv2 = inv1 = 0;
	nrg3 = nrg2 = nrg1 = nrg0 = 0;
	nrg3e = nrg2e = nrg1e = nrgE = 0;

	for (int j = 0; j<info->nz; j++)
	{
		double dcj = DielCond(j, time - 1);
		inv1 += info->e->data[j] * dcj;
		inv2 += info->h->data[j];
		inv3 += info->e->data[j] * info->e->data[j] * dcj*dcj + info->h->data[j] * info->h->data[j];

		if (realxe(j) < info->Layers[0].left)
		{
			nrg1 += Energy(j, time - 1);
			nrg1e += ElecEnergy(j, time - 1);
		}
		else if (realxe(j) <= info->Layers[info->LayerCount - 1].right)
		{
			nrg2 += Energy(j, time - 1);
			nrg2e += ElecEnergy(j, time - 1);
		}
		else
		{
			nrg3 += Energy(j, time - 1);
			nrg3e += ElecEnergy(j, time - 1);
		}
	}

	nrg0 = nrg1 + nrg2 + nrg3;
	nrgE = nrg1e + nrg2e + nrg3e;
	nrg1d = nrg1 / nrg0;
	nrg2ds = nrg2 / nrg0;
	nrg1de = nrg1e / nrgE;
	nrg2des = nrg2e / nrgE;

	nrg2d = nrg2ds + nrg1d;
	nrg2de = nrg2des + nrg1de;

	nrg1dets = nrg1e / nrg0;
	nrg2dets = nrg2e / nrg0;
	nrg3dets = nrg3e / nrg0;


	for (int j = 0; j < ARRAYSIZE(nrg_level); ++j)
	{
		if (nrg2ds > nrg_level[j])
			nrgpt[j] = time;
		if (nrg2des > nrg_level[j])
			nrgpe[j] = time;
	}
	if (time == info->TimeStamp) NrgInside = nrg2ds;

	Dump(time);
}

void NRGModule::Dump(int time)
{
	double t = realte(time);
	for (int i = 0; i < ARRAYSIZE(dmp); ++i)
	{
		fprintf(dmp[i].file, info->DumpPattern, t, dmp[i].data);
	}
}

void NRGModule::PostCalc(int time)
{
	for (int i = 0; i < ARRAYSIZE(dmp); ++i)
	{
		fclose(dmp[i].file);
	}
}