#include "module_main.h"
#include <cmath>
#include "../FFTW/fftw3.h"
#include "experiment.h"

void MainModule::Init()
{
	const auto info = experiment->medium;

	// Начальные значения
	double xe = info->realxe(0);
	for (int i = 0; i<info->nz; xe += info->hs, ++i)
	{
		info->h->data[i] = info->e->data[i] = exp(-((xe*xe) / (info->a*info->a)))*cos(info->cf*xe);
	}


	char fn[256];
	sprintf_s(fn, "e_frame%06d", 0);
	FILE *f = experiment->GetFile(fn);
	sprintf_s(fn, "e-spec_frame%06d", 0);
	FILE *fs = experiment->GetFile(fn);
	info->e->Fourier();
	info->e->Dump(f, fs, info);
	fclose(f);
	fclose(fs);
}

void MainModule::Tick(int time)
{
	const auto info = experiment->medium;

	if (time < 0) return;
	//*
	// Считаем поле
	info->e->data[0] = info->e->data[1]; // Неотражающее условие
	for (int j = 1; j < info->nz; j++)
	{
		info->e->data[j] = (1.0 / (1.0 + 0.5 * info->ts * info->Absorption(j) / info->DielCond(j))) * (info->e->data[j] * (1.0 - 0.5 * info->ts * info->Absorption(j) / info->DielCond(j)) - (info->ts / (info->hs * info->DielCond(j))) * (info->h->data[j] - info->h->data[j-1]));
	}

	info->h->data[info->nz - 1] = info->h->data[info->nz - 2]; // Неотражающее условие
	for (int j = 0; j<info->nz - 1; j++)
	{
		info->h->data[j] = info->h->data[j] - (info->e->data[j + 1] - info->e->data[j])*info->ts / info->hs;
	}

	// Dump frames
	if (time % info->DumpFrameStep == 1)
	{
		char fn[256];
		sprintf_s(fn, "e_frame%06d", time);
		FILE *f = experiment->GetFile(fn);
		sprintf_s(fn, "e-spec_frame%06d", time);
		FILE *fs = experiment->GetFile(fn);
		info->e->Fourier();
		info->e->Dump(f, fs, info);
		fclose(f);
		fclose(fs);
	}
}

void MainModule::PostCalc(int time)
{
	const auto info = experiment->medium;

	// Dump last frame
	char fn[256];
	sprintf_s(fn, "e_frame%06d", time);
	FILE *f = experiment->GetFile(fn);
	sprintf_s(fn, "e-spec_frame%06d", time);
	FILE *fs = experiment->GetFile(fn);
	info->e->Fourier();
	info->e->DumpFullPrecision(f, fs, info);
	fclose(f);
	fclose(fs);
}