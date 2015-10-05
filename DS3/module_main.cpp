#include "module_main.h"
#include <cmath>
#include "../FFTW/fftw3.h"

void MainModule::Init()
{
	StructureLeftEdge = config->GetReal("Data", "Left", 0.0);

}

void MainModule::Tick(int time)
{
	//	for (auto& RC : RecHeads)
	//	{
	//		RC->data->data[time] = e[RC->idx];
	//	}

	//*
	// Считаем поле
	for (int j = 0; j < info.nz - 1; j++)
	{
		info.e->data[j + 1] = (info.e->data[j + 1] * DielCond(j + 1, time - 1) - (info.h->data[j + 1] - info.h->data[j])*info.ts / info.hs) / DielCond(j + 1, time);
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
}