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

INIReader *config = NULL;
std::list<Module*> modules;

struct Layer
{
	double left, right, dc;
};

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

EnvInfo info;

uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
default_random_engine generator;

inline double realxe(int i) { return i*info.hs - info.lz0; }
inline double realxh(int i) { return realxe(i) + info.hs * .5; }
inline double realte(int i) { return i*info.ts; }
inline double realth(int i) { return realte(i) + info.ts * .5; }
inline int idxxe(double x) { return (int)(round((x + info.lz0) / info.hs)); }

inline double DielCond(int x, int t = 0)
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