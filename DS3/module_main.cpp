#include "module_main.h"
#include <cmath>
#include "../FFTW/fftw3.h"

void MainModule::Init()
{
	StructureLeftEdge = config->GetReal("Data", "Left", 0.0);
	DumpFrameStep = config->GetReal("Data", "FrameStep", 1e10); 

}

void MainModule::Tick(int time)
{
	if (time < 0) return;
	//*
	// Считаем поле
	info->e->data[0] = info->e->data[1]; // Неотражающее условие
	for (int j = 1; j < info->nz; j++)
	{
		info->e->data[j] = (1.0 / (1.0 + 0.5 * info->ts * Absorption(j) / DielCond(j))) * (info->e->data[j] * (1.0 - 0.5 * info->ts * Absorption(j) / DielCond(j)) - (info->ts / (info->hs * DielCond(j))) * (info->h->data[j] - info->h->data[j-1]));
	}

	info->h->data[info->nz - 1] = info->h->data[info->nz - 2]; // Неотражающее условие
	for (int j = 0; j<info->nz - 1; j++)
	{
		info->h->data[j] = info->h->data[j] - (info->e->data[j + 1] - info->e->data[j])*info->ts / info->hs;
	}

	// Dump frames
	if (time % DumpFrameStep == 1)
	{
		char* fn = new char[256];
		sprintf(fn, "e_frame%06d", time);
		FILE *f = GetFile(fn);
		sprintf(fn, "e-spec_frame%06d", time);
		FILE *fs = GetFile(fn);
		info->e->Fourier();
		info->e->Dump(f, fs);
		fclose(f);
		fclose(fs);
		delete fn;
	}
}

void MainModule::PostCalc(int time)
{

	// Dump last frame
	char* fn = new char[256];
	sprintf(fn, "e_frame%06d", time);
	FILE *f = GetFile(fn);
	sprintf(fn, "e-spec_frame%06d", time);
	FILE *fs = GetFile(fn);
	info->e->Fourier();
	info->e->DumpFullPrecision(f, fs);
	fclose(f);
	fclose(fs);
	delete fn;

	// Impulse bounds
	int l, r;
	for (int i = 0; i < info->e->GetLen(); ++i)
	{
		if (abs(info->e->data[i]) > 1e-8)
		{
			l = i;
			break;
		}

	}
	for (int i = info->e->GetLen() - 1; i >= 0 ; --i)
	{
		if (abs(info->e->data[i]) > 1e-8)
		{
			r = i;
			break;
		}

	}

	char* msg = new char[256];
	sprintf(msg, "%lf - %lf", realxe(l), realxe(r));
	Log(msg);
	delete msg;
}