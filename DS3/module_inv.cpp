#include "module_inv.h"
#include "experiment.h"
#include <cmath>

void InvModule::Init()
{
	inv[0] = new double[experiment->medium->nt];
	inv[1] = new double[experiment->medium->nt];
	inv[2] = new double[experiment->medium->nt];
	inv[3] = new double[experiment->medium->nt];

	h_cache = new double[experiment->medium->nz];

	Tick(0);
}

void InvModule::Tick(int time)
{
	const Medium *const m = experiment->medium;

	double inv3 = 0.0, inv2 = 0.0, inv1 = 0.0, inv0 = 0.0;

	for (int j = 0; j< m->nz; j++)
	{
		double dcj = m->DielCond(j);
		inv0 += m->e->data[j] * dcj;
		inv1 += m->h->data[j];
		inv2 += pow(m->e->data[j], 2) * dcj + pow(m->h->data[j], 2);
		if (time) inv3 += pow(m->e->data[j], 2) * dcj + pow(0.5 * (m->h->data[j] + h_cache[j]), 2); // if time==0 we can't average two last steps

		h_cache[j] = m->h->data[j];
	}

	inv[0][time] = inv0;
	inv[1][time] = inv1;
	inv[2][time] = inv2;
	inv[3][time] = inv3;

	// memcpy(h_cache, m->h->data, m->nz);
}


void InvModule::PostCalc(int time)
{
	char msg[512];

	inv[3][0] = inv[3][1]; // as inv[2][0] cannot be calculated, extrapolate it


	for (int i = 0; i < 4; ++i)
	{
		double average = 0.0;
		double min = INFINITY;
		double max = -INFINITY;
		for (int j = 0; j < experiment->medium->nt; ++j)
		{
			average += inv[i][j];
			if (inv[i][j] < min) min = inv[i][j];
			if (inv[i][j] > max) max = inv[i][j];
		}
		average /= experiment->medium->nt;
		sprintf_s(msg, "Inv%d: average = %.4e, max div = %.4e", i + 1, average, max(average - min, max - average));
		experiment->Log(msg);
	}

	delete[] inv[0];
	delete[] inv[1];
	delete[] inv[2];
	delete[] inv[3];
}