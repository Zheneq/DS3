#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <vector>
#include <random>
#include "module.h"
#include "field.h"
#include "../Inih/cpp/INIREADER.h"

const double SPEC_LEVEL = .001;

extern INIReader *config;
extern std::list<Module*> modules;

struct Layer
{
	double left, right, dc;
};

// Forward declaration
class field;

struct EnvInfo
{
	double lz;  // Длина среды
	double lt;  // Время симуляции 
	double lz0; // Сдвиг координатной сетки
	int    nz,  // Число шагов сетки по длине
		nt;  // Число шагов сетки по времени
	double hs,  // Шаги сетки по длине
		ts;  // Шаги сетки по времени

	double cf;  // Несущая частота волнового пакета (carrier frequency)
	double a;   // Длительность импульса

	// Параметры функции диэлектрической проницаемости
	double DP[2];
	double DPMaxDivergenceRel;

	Layer *Layers; // Границы подслоёв
	int LayerCount; // Число подслоёв
	double LayerWidth[2]; // Ширины подслоёв (чередуются)
	double LayerWidthMaxDivergenceRel;

	double eps; // Точность
	char DumpPattern[64];

	int TimeStamp; // Шаг, на котором нужно зафиксировать распределение энергии

	field *e, *h;
};

extern EnvInfo info;

extern uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
extern default_random_engine generator;

inline double realxe(int i) { return i*info.hs - info.lz0; }
inline double realxh(int i) { return realxe(i) + info.hs * .5; }
inline double realte(int i) { return i*info.ts; }
inline double realth(int i) { return realte(i) + info.ts * .5; }
inline int idxxe(double x) { return (int)(round((x + info.lz0) / info.hs)); }

inline double DielCond(int x, int t = 0);

double Energy(int x, int t);

double ElecEnergy(int x, int t);

FILE *GetFile(const char *name);