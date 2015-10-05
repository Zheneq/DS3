#pragma once
#include "global.h"

struct DumpData
{
	FILE* file;
	std::string name;
	double data;
};

double nrg_level[] = { .1, .075, .05, .025 };
int nrgpt[ARRAYSIZE(nrg_level)];
int nrgpe[ARRAYSIZE(nrg_level)];

DumpData dmp[] =
{
	{ NULL, "inv1", 0 },
	{ NULL, "inv2", 0 },
	{ NULL, "inv3", 0 },
	{ NULL, "nrg", 0 },
	{ NULL, "nrg1", 0 },
	{ NULL, "nrg2", 0 },
	{ NULL, "nrg3", 0 },
	{ NULL, "nrg1d", 0 },
	{ NULL, "nrg2d", 0 },
	{ NULL, "int", 0 },
	{ NULL, "nrgE", 0 },
	{ NULL, "nrg1E", 0 },
	{ NULL, "nrg2E", 0 },
	{ NULL, "nrg3E", 0 },
	{ NULL, "nrg1dE", 0 },
	{ NULL, "nrg2dE", 0 },
	{ NULL, "nrg1dEts", 0 },
	{ NULL, "nrg2dEts", 0 },
	{ NULL, "nrg3dEts", 0 },
	{ NULL, "nrg2ds", 0 },
	{ NULL, "nrg2des", 0 },
};

class NRGModule : Module
{
private:

	double& inv1 = dmp[0].data;
	double& inv2 = dmp[1].data;
	double& inv3 = dmp[2].data;

	// ������ �������
	double& nrg0 = dmp[3].data;

	double& nrg1 = dmp[4].data;
	double& nrg2 = dmp[5].data;
	double& nrg3 = dmp[6].data;

	// ���� �������������� ������ ������� (��� �������)
	double& nrg1d = dmp[7].data;
	double& nrg2d = dmp[8].data;
//	double& integ = dmp[9].data;

	// ������������� �������
	double& nrgE = dmp[10].data;

	double& nrg1e = dmp[11].data;
	double& nrg2e = dmp[12].data;
	double& nrg3e = dmp[13].data;

	// ���� �������������� ������������� ������� (��� �������)
	double& nrg1de = dmp[14].data;
	double& nrg2de = dmp[15].data;

	// ���� �������������� ������������� ������� �� ������
	double& nrg1dets = dmp[16].data;
	double& nrg2dets = dmp[17].data;
	double& nrg3dets = dmp[18].data;

	// ���� �������������� � ��������� �������
	double& nrg2ds = dmp[19].data;
	double& nrg2des = dmp[20].data;

	int energytime = -1;

	double NrgInside = 0;
public:
	virtual void Init();
	virtual void Tick(int time);
	virtual void PostCalc(int time);
	void Dump(int time);
};