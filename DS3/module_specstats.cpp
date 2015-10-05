#include "module_specstats.h"

void SSModule::Init()
{
	WidestSpecTimestamp = -1;
	WidestSpecWidth = 0;
}

void SSModule::Tick(int time)
{
	integral[0] = 0;
	// Считаем поле
	for (int j = 0; j < info.nz - 1; j++)
	{
		integral[0] += info.e->data[j + 1] * info.e->data[j + 1];
	}

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

	// Если изменили спектр
	// info.e->Fourier(true);
}