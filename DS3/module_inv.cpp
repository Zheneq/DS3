#include "module_inv.h"
#include "experiment.h"
#include <cmath>

void InvModule::Init()
{
	const Medium *const m = experiment->medium;

	char name[8];
	for (int i = 0; i < ARRAYSIZE(inv); ++i)
	{
		inv[i] = new double[m->nt+1];
		sprintf_s(name, "inv%02d", i);
		f[i] = experiment->GetFile(name);
	}

	h_cache = new double[m->nz];

	memcpy(h_cache, m->h->data, m->nz * sizeof(*m->h->data));
}

void InvModule::Tick(int time)
{
	const Medium *const m = experiment->medium;
	if (time <= 0) return;

	double int4 = 0.0, int3 = 0.0, int2 = 0.0, int1 = 0.0, int0 = 0.0;

	for (int j = 0; j< m->nz-1; j++)
	{
		double dcj = m->DielCond(j);
		int0 += m->e->data[j] * dcj;
		int1 += (m->h->data[j] - h_cache[j]);
		int2 += pow(m->e->data[j], 2) * dcj + pow(m->h->data[j], 2);
		int3 += pow(m->e->data[j], 2) * dcj + pow(0.5 * (m->h->data[j] + h_cache[j]), 2);
		int4 += m->h->data[j];
	}
	
	double left_e = 0.0, right_e = 0.0;
	const auto &l = experiment->observer->GetObserver("left"), &r = experiment->observer->GetObserver("right");
	for (int j = 0; j <= time; ++j)
	{
		left_e  += l.e->data[j];
		right_e += r.e->data[j];
	}
	// left_e -= .5 * l.e->data[time];
	// right_e -= .5 * r.e->data[time];

	inv[0][time] = int0;
	inv[1][time] = int1 - l.e->data[time] + r.e->data[time];
	inv[2][time] = int2;
	inv[3][time] = int3;
	inv[4][time] = int4 - left_e + right_e;

	memcpy(h_cache, m->h->data, m->nz * sizeof(*m->h->data));


}


void InvModule::PostCalc(int time)
{
	char msg[512];

	// inv[3][0] = inv[3][1]; // as inv[2][0] cannot be calculated, extrapolate it


	for (int i = 0; i < ARRAYSIZE(inv); ++i)
	{
		double average = 0.0;
		double min = INFINITY;
		double max = -INFINITY;
		for (int j = 1; j < experiment->medium->nt - 1; ++j)
		{
			average += inv[i][j];
			if (inv[i][j] < min) min = inv[i][j];
			if (inv[i][j] > max) max = inv[i][j];
			fprintf(f[i], "%.4e %.12e\n", experiment->medium->realte(j), inv[i][j]);
		}
		average /= experiment->medium->nt;
		sprintf_s(msg, "Inv%d: average = %.4e, max div = %.4e", i, average, max(average - min, max - average));
		experiment->Log(msg);
	}

	for (int i = 0; i < ARRAYSIZE(inv); ++i)
	{
		delete[] inv[i];
		fclose(f[i]);
	}
}