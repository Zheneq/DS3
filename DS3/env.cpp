#include "env.h"

EnvInfo::EnvInfo()
{
	uniform_real_distribution<double> LayerWidthDistribution, DPDistribution;
	char DumpPatternPattern[] = "%%.%df, %%.%df\n";
	double StructureLeftEdge;

	lz = config->GetReal("Data", "Length", 0.0);
	lt = config->GetReal("Data", "Time", 0.0);
	hs = config->GetReal("Data", "Step", 0.0);
	cf = config->GetReal("Data", "CarrFreq", 0.0);
	a = config->GetReal("Data", "ImpWidth", 0.0);

	DP[0] = config->GetReal("Data", "Eps1", 0.0);
	DP[1] = config->GetReal("Data", "Eps2", 0.0);
	DPMaxDivergenceRel = config->GetReal("Data", "EpsMaxDivergenceRel", 0.1);

	StructureLeftEdge = config->GetReal("Data", "Left", 0.0);
	LayerWidth[0] = config->GetReal("Data", "LayerWidth1", 0.0);
	LayerWidth[1] = config->GetReal("Data", "LayerWidth2", 0.0);
	LayerCount = config->GetInteger("Data", "LayerCount", 0);
	LayerWidthMaxDivergenceRel = config->GetReal("Data", "LayerWidthMaxDivergenceRel", 0.1);

	LayerWidthDistribution = std::uniform_real_distribution<double>(1.0 - LayerWidthMaxDivergenceRel, 1.0 + LayerWidthMaxDivergenceRel);
	DPDistribution = std::uniform_real_distribution<double>(1.0 - DPMaxDivergenceRel, 1.0 + DPMaxDivergenceRel);

	// Инициализация
	lz0 = lz / 2;
	ts = hs;

	nz = (int)(lz / hs);
	nt = (int)(lt / ts);
	++nt;


	int dig = (int)ceil(log(1 / (hs*hs)) / log(10)) + 4;
	eps = pow(.1, dig) / 2 - 1e-10;
	sprintf(DumpPattern, DumpPatternPattern, dig, dig);

	// Генерирование слоя
	Layers = new Layer[LayerCount];
	int i;

	Layers[0].left = StructureLeftEdge;
	Layers[0].right = Layers[0].left + LayerWidth[0] * LayerWidthDistribution(*gen);
	Layers[0].dc = DP[0] * DPDistribution(*gen);
	Layers[0].dc = Layers[0].dc < 1.0 ? 1.0 : Layers[0].dc;

	for (i = 0; i < (LayerCount - 1) / 2; ++i)
	{
		Layers[2 * i + 1].left = Layers[2 * i].right;
		Layers[2 * i + 1].right = Layers[2 * i + 1].left + LayerWidth[1] * LayerWidthDistribution(*gen);
		Layers[2 * i + 1].dc = DP[1] * DPDistribution(*gen);
		Layers[2 * i + 1].dc = Layers[2 * i + 1].dc < 1.0 ? 1.0 : Layers[2 * i + 1].dc;

		Layers[2 * i + 2].left = Layers[2 * i + 1].right;
		Layers[2 * i + 2].right = Layers[2 * i + 2].left + LayerWidth[0] * LayerWidthDistribution(*gen);
		Layers[2 * i + 2].dc = DP[0] * DPDistribution(*gen);
		Layers[2 * i + 2].dc = Layers[2 * i + 2].dc < 1.0 ? 1.0 : Layers[2 * i + 2].dc;
	}
	if (!(LayerCount % 2))
	{
		Layers[LayerCount - 1].left = Layers[LayerCount - 2].right;
		Layers[LayerCount - 1].right = Layers[LayerCount - 1].left + LayerWidth[1] * LayerWidthDistribution(*gen);
		Layers[LayerCount - 1].dc = DP[1] * DPDistribution(*gen);
	}
	
	printf("Layers: \n");
	for (int i = 0; i < LayerCount; ++i)
	{
		printf("\t%.2f ", Layers[i].right - Layers[i].left);
	}
	printf("\n");
	for (int i = 0; i < LayerCount; ++i)
	{
		printf("\t%.2f ", Layers[i].dc);
	}
	printf("\n");

	// Инициализация классов поля
	h = new field();
	h->Init(nz, FFTW_MEASURE);
	e = new field();
	e->Init(nz, FFTW_MEASURE);

	h_cache = e_cache = NULL;
}