#include "module.h"
#include <random>

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

	const double *e, *h;
};

class MainModule : Module
{
private:
	EnvInfo info;

	uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
	default_random_engine generator;

	double StructureLeftEdge; // Левая граница слоя

	double *e, *h;
public:
	const EnvInfo& Info() { return info; }

	virtual void Init();
	virtual void Tick();

	virtual list<DataMapping> GenInputList();
};
