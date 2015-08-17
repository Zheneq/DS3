#include "module_main.h"
#include <cmath>
#include "../FFTW/fftw3.h"

MainModule::MainModule() : Module()
{

}

list<DataMapping> MainModule::GenInputList()
{
	list<DataMapping> res;

	DataMapping Map[] =
	{
		{ "Length", "%lf", &info.lz, NULL },
		{ "Time", "%lf", &info.lt, NULL },
		{ "Step", "%lf", &info.hs, NULL },
		{ "CarrFreq", "%lf", &info.cf, NULL },
		{ "ImpWidth", "%lf", &info.a, NULL },


		{ "Eps1", "%lf", &info.DP[0], NULL },
		{ "Eps2", "%lf", &info.DP[1], NULL },
		{ "EpsMaxDivergenceRel", "%lf", &info.DPMaxDivergenceRel, "0.1" },

		{ "Left", "%lf", &StructureLeftEdge, NULL },
		{ "LayerWidth1", "%lf", &info.LayerWidth[0], NULL },
		{ "LayerWidth2", "%lf", &info.LayerWidth[1], NULL },
		{ "LayerCount", "%d", &info.LayerCount, NULL },
		{ "LayerWidthMaxDivergenceRel", "%lf", &info.LayerWidthMaxDivergenceRel, "0.1" }
	};
	
	for each(auto& m in Map)
	{
		res.push_back(m);
	}

	return res;
}

void MainModule::Init()
{
	char DumpPatternPattern[] = "%%.%df, %%.%df\n";
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

	info.h = h = (double*)fftw_malloc(nz*sizeof(double));
	info.e = e = (double*)fftw_malloc(nz*sizeof(double));

	//	printf("Choosing best algorithm for Fourier transform... ");
	Fourier(CP_INIT);
	//	printf("Done!\n");

	// Начальные значения
	double xe = realxe(0), xh = realxh(0);
	for (int i = 0; i<nz; xe += hs, xh += hs, ++i)
	{
		h[i] = e[i] = exp(-((xe*xe) / (a*a)))*cos(cf*xe);
	}


	WidestSpecTimestamp = -1;
	WidestSpecWidth = 0;

	for (int i = 0; i < ARRAYSIZE(nrg_level); ++i)
	{
		nrgpt[i] = -1;
		nrgpe[i] = -1;
	}
}