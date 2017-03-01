#pragma once
#include "global.h"
#define _USE_MATH_DEFINES
#include <math.h>

struct Parameter
{
	char name[64];

	Parameter(const char *_name)
	{
		strcpy_s(name, _name);
	}

	virtual void Read(INIReader *config) = 0;
};

struct IntParameter : public Parameter
{
	int value;

	IntParameter(const char *_name, int default) : Parameter(_name)
	{
		value = default;
	}

	void Read(INIReader *config)
	{
		value = config->GetInteger("Data", name, value);
	}
};

struct RealParameter : public Parameter
{
	double value;

	RealParameter(const char *_name, double default) : Parameter(_name)
	{
		value = default;
	}

	void Read(INIReader *config)
	{
		value = config->GetReal("Data", name, value);
	}
};

class Medium
{
	std::vector<IntParameter> IntParams;
	std::vector<RealParameter> RealParams;

	int GetInt(const char *name)
	{
		for (auto p : IntParams)
			if (strcmp(name, p.name))
				return p.value;

		throw "Undeclared parameter.";
		return 0;
	}

	double GetReal(const char *name)
	{
		for (auto p : RealParams)
			if (strcmp(name, p.name))
				return p.value;

		throw "Undeclared parameter.";
		return 0;
	}

public:
	Medium();
	void Load(INIReader *config);
	void Init();


	double realxe(int i) { return i*hs - lz0; }
	double realxh(int i) { return realxe(i) + hs * .5; }
	double realte(int i) { return i*ts; }
	double realth(int i) { return realte(i) + ts * .5; }
	double realspec(int i) { return 2 * M_PI * i / lz; }
	double realspect(int i) { return 2 * M_PI * i / lt; }
	int idxxe(double x) { return (int)(round((x + lz0) / hs)); }

	double DielCond(int x);
	double Absorption(int x);
	double Energy(int x);
	double ElecEnergy(int x);


	double lz;  // ����� �����
	double lt;  // ����� ��������� 
	double lz0; // ����� ������������ �����
	int    nz;  // ����� ����� ����� �� �����
	int    nt;  // ����� ����� ����� �� �������
	double hs;  // ���� ����� �� �����
	double ts;  // ���� ����� �� �������

	double cf;  // ������� ������� ��������� ������ (carrier frequency)
	double a;   // ������������ ��������

	// ��������� ������� ��������������� �������������
	double DP[2];
	double DPMaxDivergenceRel;

	// ���������� (��. double Absorption(int x) @ ds3.cpp)
	double AbsorbCoef, AbsorbCenter, AbsorbHalfWidth;

	Layer *Layers; // ������� �������
	int LayerCount; // ����� �������
	double LayerWidth[2]; // ������ ������� (����������)
	double LayerWidthMaxDivergenceRel;


	field *e, *h;
};