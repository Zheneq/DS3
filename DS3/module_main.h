#include "module.h"
#include <random>

struct Layer
{
	double left, right, dc;
};

struct EnvInfo
{
	double lz;  // ����� �����
	double lt;  // ����� ��������� 
	double lz0; // ����� ������������ �����
	int    nz,  // ����� ����� ����� �� �����
		   nt;  // ����� ����� ����� �� �������
	double hs,  // ���� ����� �� �����
		   ts;  // ���� ����� �� �������

	double cf;  // ������� ������� ��������� ������ (carrier frequency)
	double a;   // ������������ ��������

	// ��������� ������� ��������������� �������������
	double DP[2];
	double DPMaxDivergenceRel;

	Layer *Layers; // ������� �������
	int LayerCount; // ����� �������
	double LayerWidth[2]; // ������ ������� (����������)
	double LayerWidthMaxDivergenceRel;

	double eps; // ��������
	char DumpPattern[64];

	const double *e, *h;
};

class MainModule : Module
{
private:
	EnvInfo info;

	uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
	default_random_engine generator;

	double StructureLeftEdge; // ����� ������� ����

	double *e, *h;
public:
	const EnvInfo& Info() { return info; }

	virtual void Init();
	virtual void Tick();

	virtual list<DataMapping> GenInputList();
};
