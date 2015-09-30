
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <vector>
#include <random>
#include "global.h"

using namespace std;


void Load(int argc, char **argv)
{
	const unsigned len = 512;
	char Path[len];

	// Если не задан файл конфигурации, ищем дефолтный
	if (argc == 1)
	{
		strcpy(Path, argv[0]); //strcpy_s(Path, strlen(argv[0]), argv[0]);
		for (int i = strlen(argv[0]); i; --i) if (Path[i] == '\\' || Path[i] == '/') { Path[i] = '\0'; break; }
		strcat(Path, "\\default.ini");
	}
	else
	{
		strcpy(Path, argv[1]);
	}
	printf("%s\n", Path);

	if (config) delete config;
	config = new INIReader(Path);

	if (config->ParseError() < 0) {
		throw("Can't load ini file");
	}
	
	//std::cout << "Config loaded from 'test.ini': version="
	//	<< reader.GetInteger("protocol", "version", -1) << ", name="
	//	<< reader.Get("user", "name", "UNKNOWN") << ", email="
	//	<< reader.Get("user", "email", "UNKNOWN") << ", pi="
	//	<< reader.GetReal("user", "pi", -1) << ", active="
	//	<< reader.GetBoolean("user", "active", true) << "\n";
}

void Init(int argc, char **argv)
{
	char DumpPatternPattern[] = "%%.%df, %%.%df\n";
	double StructureLeftEdge;

	info.lz = config->GetReal("Data", "Length", 0.0);
	info.lt = config->GetReal("Data", "Time", 0.0);
	info.hs = config->GetReal("Data", "Step", 0.0);
	info.cf = config->GetReal("Data", "CarrFreq", 0.0);
	info.a = config->GetReal("Data", "ImpWidth", 0.0);

	info.DP[0] = config->GetReal("Data", "Eps1", 0.0);
	info.DP[1] = config->GetReal("Data", "Eps2", 0.0);
	info.cf = config->GetReal("Data", "EpsMaxDivergenceRel", 0.1);

	StructureLeftEdge = config->GetReal("Data", "Left", 0.0);
	info.LayerWidth[0] = config->GetReal("Data", "LayerWidth1", 0.0);
	info.LayerWidth[1] = config->GetReal("Data", "LayerWidth2", 0.0);
	info.LayerCount = config->GetInteger("Data", "LayerCount", 0);
	info.LayerWidthMaxDivergenceRel = config->GetReal("Data", "LayerWidthMaxDivergenceRel", 0.1);

	LayerWidthDistribution = std::uniform_real_distribution<double>(1.0 - info.LayerWidthMaxDivergenceRel, 1.0 + info.LayerWidthMaxDivergenceRel);
	DPDistribution = std::uniform_real_distribution<double>(1.0 - info.DPMaxDivergenceRel, 1.0 + info.DPMaxDivergenceRel);

	// Инициализация
	info.lz0 = info.lz / 2;
	info.ts = info.hs;

	info.nz = (int)(info.lz / info.hs);
	info.nt = (int)(info.lt / info.ts);
	++info.nt;

	int dig = (int)ceil(log(1 / (info.hs*info.hs)) / log(10)) + 1;
	info.eps = pow(.1, dig) / 2 - 1e-10;
	sprintf(info.DumpPattern, DumpPatternPattern, dig, dig);

	// Генерирование слоя
	info.Layers = new Layer[info.LayerCount];
	int i;

	info.Layers[0].left = StructureLeftEdge;
	info.Layers[0].right = info.Layers[0].left + info.LayerWidth[0] * LayerWidthDistribution(generator);
	info.Layers[0].dc = info.DP[0] * DPDistribution(generator);
	info.Layers[0].dc = info.Layers[0].dc < 1.0 ? 1.0 : info.Layers[0].dc;

	for (i = 0; i < (info.LayerCount - 1) / 2; ++i)
	{
		info.Layers[2 * i + 1].left = info.Layers[2 * i].right;
		info.Layers[2 * i + 1].right = info.Layers[2 * i + 1].left + info.LayerWidth[1] * LayerWidthDistribution(generator);
		info.Layers[2 * i + 1].dc = info.DP[1] * DPDistribution(generator);
		info.Layers[2 * i + 1].dc = info.Layers[2 * i + 1].dc < 1.0 ? 1.0 : info.Layers[2 * i + 1].dc;

		info.Layers[2 * i + 2].left = info.Layers[2 * i + 1].right;
		info.Layers[2 * i + 2].right = info.Layers[2 * i + 2].left + info.LayerWidth[0] * LayerWidthDistribution(generator);
		info.Layers[2 * i + 2].dc = info.DP[0] * DPDistribution(generator);
		info.Layers[2 * i + 2].dc = info.Layers[2 * i + 2].dc < 1.0 ? 1.0 : info.Layers[2 * i + 2].dc;
	}
	if (!(info.LayerCount % 2))
	{
		info.Layers[info.LayerCount - 1].left = info.Layers[info.LayerCount - 2].right;
		info.Layers[info.LayerCount - 1].right = info.Layers[info.LayerCount - 1].left + info.LayerWidth[1] * LayerWidthDistribution(generator);
		info.Layers[info.LayerCount - 1].dc = info.DP[1] * DPDistribution(generator);
	}


	//
	//	RecHeads.push_back(new RecHead(idxxe(obj_raw.left - 10), nt));
	//	RecHeads.push_back(new RecHead(idxxe(obj_raw.right + 10), nt));

	info.h = new field();
	info.h->Init(info.nz);
	info.e = new field();
	info.e->Init(info.nz);

	//	printf("Choosing best algorithm for Fourier transform... ");
	// Fourier(CP_INIT);
	//	printf("Done!\n");

	// Начальные значения
	double xe = realxe(0), xh = realxh(0);
	for (int i = 0; i<info.nz; xe += info.hs, xh += info.hs, ++i)
	{
		info.h->data[i] = info.e->data[i] = exp(-((xe*xe) / (info.a*info.a)))*cos(info.cf*xe);
	}
}

int main(int argc, char **argv)
{
	try
	{
		Load(argc, argv);
		Init(argc, argv);
		for (auto m : modules)
			m->Init();
		
	}
	catch (char *error)
	{
		cerr << error << endl;
		return 1;
	}

	return 0;
}