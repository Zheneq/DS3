#include "module_inv.h"
#include "experiment.h"

void InvModule::Init()
{
	inv[0] = new double[experiment->medium->nt];
	inv[1] = new double[experiment->medium->nt];
	inv[2] = new double[experiment->medium->nt];
	Tick(0);
}

void InvModule::Tick(int time)
{
	double inv2 = 0.0, inv1 = 0.0, inv0 = 0.0;

	for (int j = 0; j<experiment->medium->nz; j++)
	{
		double dcj = experiment->medium->DielCond(j);
		inv0 += experiment->medium->e->data[j] * dcj;
		inv1 += experiment->medium->h->data[j];
		inv2 += experiment->medium->e->data[j] * experiment->medium->e->data[j] * dcj + experiment->medium->h->data[j] * experiment->medium->h->data[j];
	}

	inv[0][time] = inv0;
	inv[1][time] = inv1;
	inv[2][time] = inv2;
}


void InvModule::PostCalc(int time)
{
	double average[3], max[3], min[3];
	char msg[256];


	for (int i = 0; i < 3; ++i)
	{
		average[i] = 0.0;
		min[i] = INFINITY;
		max[i] = -INFINITY;
		for (int j = 0; j < experiment->medium->nt; ++j)
		{
			average[i] += inv[i][j];
			if (inv[i][j] < min[i]) min[i] = inv[i][j];
			if (inv[i][j] > max[i]) max[i] = inv[i][j];
		}
		average[i] /= experiment->medium->nt;
		sprintf_s(msg, "Inv%d: average = %lf, max div = %lf", i + 1, average[i], max(average[i] - min[i], max[i] - average[i]));
		experiment->Log(msg);
	}

	delete[] inv[0];
	delete[] inv[1];
	delete[] inv[2];
}