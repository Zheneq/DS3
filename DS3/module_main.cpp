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
	for (int j = 0; j < info->nz - 1; j++)
	{
		info->e->data[j + 1] = (info->e->data[j + 1] * DielCond(j + 1, time - 1) - (info->h->data[j + 1] - info->h->data[j])*info->ts / info->hs) / DielCond(j + 1, time);
	}
	info->e->data[0] = 0;

	for (int j = 0; j<info->nz - 1; j++)
	{
		info->h->data[j] = info->h->data[j] - (info->e->data[j + 1] - info->e->data[j])*info->ts / info->hs;
	}
	info->h->data[info->nz - 1] = 0;

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
	info->e->Dump(f, fs);
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