#include "global.h"

INIReader *config = NULL;
std::list<Module*> modules;
EnvInfo info;

uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
default_random_engine generator;

inline double DielCond(int x, int t)
{
	register double rx = realxe(x);

	// Если мы вне слоя, сразу выходим
	if ((rx<info.Layers[0].left - info.hs) || (rx>info.Layers[info.LayerCount - 1].right + info.hs))
		return 1;
	for (int i = 0; i < info.LayerCount; ++i)
	{
		// Если мы попали на границу
		if (abs(rx - info.Layers[i].left) < info.hs*.5 || abs(rx - info.Layers[i].right) < info.hs*.5)
			return .5*(DielCond(x - 1, t) + DielCond(x + 1, t));
		// Если мы попали в подслой
		if (rx > info.Layers[i].left && rx < info.Layers[i].right)
			return info.Layers[i].dc;
	}
	// Если мы никуда не попали
	return 1;
}

double Energy(int x, int t)
{
	return .5 * (DielCond(x, t) * info.e->data[x] * info.e->data[x] + info.h->data[x] * info.h->data[x]);
}

double ElecEnergy(int x, int t)
{
	return .5 * DielCond(x, t) * info.e->data[x] * info.e->data[x];
}

FILE *GetFile(const char *name)
{
	// TODO
	FILE *f = fopen("", "wt");
	return f;
}