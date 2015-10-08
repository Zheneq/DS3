#include "global.h"

INIReader *config = NULL;
EnvInfo *info = NULL;

// TODO: inline??
double realxe(int i) { return i*info->hs - info->lz0; }
double realxh(int i) { return realxe(i) + info->hs * .5; }
double realte(int i) { return i*info->ts; }
double realth(int i) { return realte(i) + info->ts * .5; }
int idxxe(double x) { return (int)(round((x + info->lz0) / info->hs)); }

inline double DielCond(int x, int t)
{
	register double rx = realxe(x);

	// Если мы вне слоя, сразу выходим
	if ((rx<info->Layers[0].left - info->hs) || (rx>info->Layers[info->LayerCount - 1].right + info->hs))
		return 1;
	for (int i = 0; i < info->LayerCount; ++i)
	{
		// Если мы попали на границу
		if (abs(rx - info->Layers[i].left) < info->hs*.5 || abs(rx - info->Layers[i].right) < info->hs*.5)
			return .5*(DielCond(x - 1, t) + DielCond(x + 1, t));
		// Если мы попали в подслой
		if (rx > info->Layers[i].left && rx < info->Layers[i].right)
			return info->Layers[i].dc;
	}
	// Если мы никуда не попали
	return 1;
}

double Energy(int x, int t)
{
	return .5 * (DielCond(x, t) * info->e->data[x] * info->e->data[x] + info->h->data[x] * info->h->data[x]);
}

double ElecEnergy(int x, int t)
{
	return .5 * DielCond(x, t) * info->e->data[x] * info->e->data[x];
}

FILE *GetFile(const char *name)
{
	char DumpPath[512];
	sprintf(DumpPath, "%s/E%03d/%s.txt", config->Get("Data", "DumpPath", "").c_str(), ExperimentNum, name);
	FILE *f = fopen(DumpPath, "wt");

	if (!f)
	{
		char Error[544];
		sprintf(Error, "Cannot create file: %s", DumpPath);
		throw(DumpPath);
	}

	return f;
}