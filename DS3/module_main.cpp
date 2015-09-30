#include "module_main.h"
#include <cmath>
#include "../FFTW/fftw3.h"

MainModule::MainModule() : Module()
{

}

void MainModule::Init()
{
	StructureLeftEdge = config->GetReal("Data", "Left", 0.0);

	WidestSpecTimestamp = -1;
	WidestSpecWidth = 0;

	for (int i = 0; i < ARRAYSIZE(nrg_level); ++i)
	{
		nrgpt[i] = -1;
		nrgpe[i] = -1;
	}
}

void MainModule::Tick(int time)
{
	// Считаем инварианты
	inv3 = inv2 = inv1 = 0;
	nrg3 = nrg2 = nrg1 = nrg0 = 0;
	nrg3e = nrg2e = nrg1e = nrgE = 0;

	for (int j = 0; j<info.nz; j++)
	{
		double dcj = DielCond(j, time - 1);
		inv1 += info.e->data[j] * dcj;
		inv2 += info.h->data[j];
		inv3 += info.e->data[j] * info.e->data[j] * dcj*dcj + info.h->data[j] * info.h->data[j];

		if (realxe(j) < info.Layers[0].left)
		{
			nrg1 += Energy(j, time - 1);
			nrg1e += ElecEnergy(j, time - 1);
		}
		else if (realxe(j) <= info.Layers[info.LayerCount - 1].right)
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

//	for (auto& RC : RecHeads)
//	{
//		RC->data->data[time] = e[RC->idx];
//	}

	for (int j = 0; j < ARRAYSIZE(nrg_level); ++j)
	{
		if (nrg2ds > nrg_level[j])
			nrgpt[j] = time;
		if (nrg2des > nrg_level[j])
			nrgpe[j] = time;
	}
	if (time == info.TimeStamp) NrgInside = nrg2ds;

	//*
	integral[0] = 0;
	// Считаем поле
	for (int j = 0; j < info.nz - 1; j++)
	{
		info.e->data[j + 1] = (info.e->data[j + 1] * DielCond(j + 1, time - 1) - (info.h->data[j + 1] - info.h->data[j])*info.ts / info.hs) / DielCond(j + 1, time);
		integral[0] += info.e->data[j + 1] * info.e->data[j + 1];
	}
	info.e->data[0] = 0;

	for (int j = 0; j<info.nz - 1; j++)
	{
		info.h->data[j] = info.h->data[j] - (info.e->data[j + 1] - info.e->data[j])*info.ts / info.hs;
	}
	info.h->data[info.nz - 1] = 0;
	//*/
	info.e->Fourier();
	// info.h->Fourier();
	// Делаем что-нибудь со спектром

	// Ширина спектра и интеграл
	integral[1] = 0;
	int w = -1;
	for (int j = 0; j < info.nz / 2 + 1; ++j)
	{
		integral[1] += info.e->spec[j];
		if (info.e->spec[j] >= SPEC_LEVEL) w = j;
	}
	integ = integral[0] - integral[1] + 0.5*info.e->spec[0];
	if (w > WidestSpecWidth)
	{
		WidestSpecWidth = w;
		WidestSpecTimestamp = time;
	}
	//*/
	// Dump();

	//	Fourier(CP_RUN, true);
}