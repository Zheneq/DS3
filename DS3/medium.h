#pragma once
#include "../Inih/cpp/INIREADER.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <random>
#include "configurer.h"

extern std::default_random_engine *gen;
class field;
class Experiment;

struct Layer
{
	double left, right, dc;
};

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
	Experiment *experiment;

	void Log(const char* msg);

	int GetInt(const char *name)
	{
		for (auto p : IntParams)
			if (!strcmp(name, p.name))
				return p.value;

		throw "Undeclared parameter.";
		return 0;
	}

	double GetReal(const char *name)
	{
		for (auto p : RealParams)
			if (!strcmp(name, p.name))
				return p.value;

		throw "Undeclared parameter.";
		return 0;
	}

public:
	Medium(Experiment* const experiment);
	~Medium();
	void Load(const Configurer &config);
	void Init();


	double realxe(int i) const { return i*hs - lz0; }
	double realxh(int i) const { return realxe(i) + hs * .5; }
	double realte(int i) const { return i*ts; }
	double realth(int i) const { return realte(i) + ts * .5; }
	double realspec(int i) const { return 2 * M_PI * i / lz; }
	double realspect(int i) const { return 2 * M_PI * i / lt; }
	int idxxe(double x) const { return (int)(round((x + lz0) / hs)); }

	double DielCond(int x) const;
	double Absorption(int x) const;
	double Energy(int x) const;
	double ElecEnergy(int x) const;


	double lz;  // Длина среды
	double lt;  // Время симуляции 
	double lz0; // Сдвиг координатной сетки
	int    nz;  // Число шагов сетки по длине
	int    nt;  // Число шагов сетки по времени
	double hs;  // Шаги сетки по длине
	double ts;  // Шаги сетки по времени

	double cf;  // Несущая частота волнового пакета (carrier frequency)
	double a;   // Длительность импульса

	// Параметры функции диэлектрической проницаемости
	double DP[2];
	double DPMaxDivergenceRel;

	// Поглощение (см. double Absorption(int x) @ ds3.cpp)
	double AbsorbCoef, AbsorbCenter, AbsorbHalfWidth;

	Layer *Layers; // Границы подслоёв
	int LayerCount; // Число подслоёв
	double LayerWidth[2]; // Ширины подслоёв (чередуются)
	double LayerWidthMaxDivergenceRel;

	int DumpFrameStep;


	field *e, *h;
};