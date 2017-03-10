#include "medium.h"
#include "field.h"
#include <random>

Medium::Medium()
{
	// Объявление параметров
	RealParams.emplace_back("Length", 0.0);
	RealParams.emplace_back("Time", 0.0);
	RealParams.emplace_back("Step", 0.0);
	RealParams.emplace_back("CarrFreq", 0.0);
	RealParams.emplace_back("ImpWidth", 0.0);


	RealParams.emplace_back("Eps1", 0.0);
	RealParams.emplace_back("Eps2", 0.0);
	RealParams.emplace_back("EpsMaxDivergenceRel", 0.1);

	IntParams.emplace_back("LayerCount", 0);

	RealParams.emplace_back("AbsorbHalfWidth", 0.0);

	RealParams.emplace_back("Left", 0.0);
	RealParams.emplace_back("LayerWidth1", 0.0);
	RealParams.emplace_back("LayerWidth2", 0.0);
	IntParams.emplace_back("LayerCount", 0);
	RealParams.emplace_back("LayerWidthMaxDivergenceRel", 0.1);
	RealParams.emplace_back("AbsorbCoef", 1.0);
	// RealParams.emplace_back("AbsorbCenter", 0.0);
	IntParams.emplace_back("AbsorbLayer", -1);
	// ...

	IntParams.emplace_back("FrameStep", 1000000000);
}

void Medium::Load(INIReader *config)
{
	for (auto p : IntParams) p.Read(config);
	for (auto p : RealParams) p.Read(config);
}

void Medium::Init()
{

	// Values 
	lz = GetReal("Length");
	lt = GetReal("Time");
	hs = GetReal("Step");
	cf = GetReal("CarrFreq");
	a = GetReal("ImpWidth");

	DP[0] = GetReal("Eps1");
	DP[1] = GetReal("Eps2");
	DPMaxDivergenceRel = GetReal("EpsMaxDivergenceRel");

	AbsorbHalfWidth = GetReal("AbsorbHalfWidth");

	double StructureLeftEdge = GetReal("Left");
	LayerWidth[0] = GetReal("LayerWidth1");
	LayerWidth[1] = GetReal("LayerWidth2");
	LayerCount = GetInt("LayerCount");
	LayerWidthMaxDivergenceRel = GetReal("LayerWidthMaxDivergenceRel");
	AbsorbCoef = GetReal("AbsorbCoef");
	// AbsorbCenter = GetReal("AbsorbCenter");
	int AbsorbLayer = GetInt("AbsorbLayer");

	DumpFrameStep = GetInt("FrameStep");

	// Generation
	lz0 = lz / 2;
	ts = hs;

	nz = (int)(lz / hs);
	nt = (int)(lt / ts) + 1;



	// Генерирование слоя
	std::uniform_real_distribution<double> LayerWidthDistribution = std::uniform_real_distribution<double>(1.0 - LayerWidthMaxDivergenceRel, 1.0 + LayerWidthMaxDivergenceRel);
	std::uniform_real_distribution<double> DPDistribution = std::uniform_real_distribution<double>(1.0 - DPMaxDivergenceRel, 1.0 + DPMaxDivergenceRel);
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

	if (AbsorbLayer >= 0)
	{
		double add = LayerWidth[0] - (Layers[AbsorbLayer].right - Layers[AbsorbLayer].left);

		for (int i = AbsorbLayer; i < LayerCount; ++i)
		{
			Layers[i].left = Layers[i - 1].right;
			Layers[i].right += add;
		}

		AbsorbCenter = .5 * (Layers[AbsorbLayer].right + Layers[AbsorbLayer].left);
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
	h->Init(nz, FFTW_ESTIMATE);
	e = new field();
	e->Init(nz, FFTW_ESTIMATE);
}

 double Medium::DielCond(int x/*, int t*/)
{
	register double rx = realxe(x);

	// Если мы вне слоя, сразу выходим
	if ((rx<Layers[0].left - hs) || (rx>Layers[LayerCount - 1].right + hs))
		return 1;
	for (int i = 0; i < LayerCount; ++i)
	{
		// Если мы попали на границу
		if (abs(rx - Layers[i].left) < hs*.5 || abs(rx - Layers[i].right) < hs*.5)
			return .5*(DielCond(x - 1/*, t*/) + DielCond(x + 1/*, t*/));
		// Если мы попали в подслой
		if (rx > Layers[i].left && rx < Layers[i].right)
			return Layers[i].dc;
	}
	// Если мы никуда не попали
	return 1;
}

double Medium::Absorption(int x)
{
	// gauss
	double rx = realxe(x);
	return AbsorbCoef * exp(-pow((rx - AbsorbCenter) / AbsorbHalfWidth, 2));

	// rectangle
	/*
	int l = idxxe(info->AbsorbCenter - info->AbsorbHalfWidth), r = idxxe(info->AbsorbCenter + info->AbsorbHalfWidth);

	if (x > l && x < r)
	return info->AbsorbCoef;

	if (x == l || x == r)
	return .5 * info->AbsorbCoef;

	return 0.0;
	//*/
}

double Medium::Energy(int x)
{
	return .5 * (DielCond(x) * e->data[x] * e->data[x] + h->data[x] * h->data[x]);
}

double Medium::ElecEnergy(int x)
{
	return .5 * DielCond(x) * e->data[x] * e->data[x];
}