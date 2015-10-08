#pragma once
#include "global.h"

// Forward declaration
struct Layer;
class field;

class EnvInfo
{
private:

public:
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

	field *e, *h, *e_cache, *h_cache;

	EnvInfo();
};