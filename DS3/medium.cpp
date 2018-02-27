#include "medium.h"
#include "field.h"
#include "experiment.h"
#include <random>

Medium::Medium(Experiment* const _experiment) : Layers(nullptr), e(nullptr), h(nullptr), experiment(_experiment)
{
	// Объявление параметров
	RealParams.emplace_back("Length", 0.0);
	RealParams.emplace_back("Time", 0.0);
	RealParams.emplace_back("Step", 0.0);
	RealParams.emplace_back("CarrFreq", 0.0);
	RealParams.emplace_back("ImpHalfWidth", 0.0);


	RealParams.emplace_back("Eps0", 0.0);
	RealParams.emplace_back("Eps1", 0.0);
	RealParams.emplace_back("EpsMaxDivergenceRel", 0.1);

	IntParams.emplace_back("LayerCount", 0);

	RealParams.emplace_back("Left", 0.0);
	RealParams.emplace_back("LayerWidth0", 0.0);
	RealParams.emplace_back("LayerWidth1", 0.0);
	RealParams.emplace_back("LayerWidthMaxDivergenceRel", 0.1);

	IntParams.emplace_back("AbsorbLayer", -1);
	RealParams.emplace_back("AbsorbHalfWidth", 0.0);
	RealParams.emplace_back("AbsorbCoef", 1.0);
	// RealParams.emplace_back("AbsorbCenter", 0.0);
	// ...

	IntParams.emplace_back("FrameStep", 1000000000);
}

Medium::~Medium()
{
	if (Layers) delete[] Layers;
	if (e) delete e;
	if (h) delete h;
}

void Medium::Log(const char* msg)
{
	if (experiment) experiment->Log(msg);
}

// TODO: Переделать эту фунцию по-нормальному
void Medium::Load(const Configurer &config)
{
	for (auto &p : IntParams) p.Read(config.base);
	for (auto &p : RealParams) p.Read(config.base);

	if (config.over)
	{
		for (auto &p : IntParams) p.Read(config.over);
		for (auto &p : RealParams) p.Read(config.over);
	}

	// Log
	char msg[256];
	if (experiment)
	{
		Log("Actual parameters:");
		for (auto &p : IntParams)
		{
			sprintf_s(msg, "%s = %d", p.name, p.value);
			Log(msg);
		}
		for (auto &p : RealParams)
		{
			sprintf_s(msg, "%s = %lf", p.name, p.value);
			Log(msg);
		}
		Log("");
	}
}

void Medium::Init()
{

	// Values 
	// lz = GetReal("Length");
	lt = GetReal("Time");
	hs = GetReal("Step");
	cf = GetReal("CarrFreq");
	a = GetReal("ImpHalfWidth");

	DP[0] = GetReal("Eps0");
	DP[1] = GetReal("Eps1");
	DPMaxDivergenceRel = GetReal("EpsMaxDivergenceRel");

	AbsorbHalfWidth = GetReal("AbsorbHalfWidth");

	double StructureLeftEdge = GetReal("Left");
	LayerWidth[0] = GetReal("LayerWidth0");
	LayerWidth[1] = GetReal("LayerWidth1");
	LayerCount = GetInt("LayerCount");
	LayerWidthMaxDivergenceRel = GetReal("LayerWidthMaxDivergenceRel");
	AbsorbCoef = GetReal("AbsorbCoef");
	// AbsorbCenter = GetReal("AbsorbCenter");
	int AbsorbLayer = GetInt("AbsorbLayer");

	DumpFrameStep = GetInt("FrameStep");

	// Generation
	double requiredLength = GetReal("ImpHalfWidth") * 5 + StructureLeftEdge + max(LayerWidth[0], LayerWidth[1]) * (LayerWidthMaxDivergenceRel + 1) * LayerCount;
	int requiredPoints = (int)ceil(requiredLength / hs);

	nz = (int)pow(2, (int)log2(requiredPoints) + 1);
	lz = nz * hs;


	lz0 = (lz - requiredLength) * .5 + GetReal("ImpHalfWidth") * 5;
	ts = hs;

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
		double add = LayerWidth[AbsorbLayer % 2] - (Layers[AbsorbLayer].right - Layers[AbsorbLayer].left);

		for (int i = AbsorbLayer; i < LayerCount; ++i)
		{
			Layers[i].left = Layers[i - 1].right;
			Layers[i].right += add;
		}

		AbsorbCenter = .5 * (Layers[AbsorbLayer].right + Layers[AbsorbLayer].left);
	}
	else
	{
		AbsorbCoef = 0.0;
	}

	char msg[256];
	Log("Layers:");
	for (int i = 0; i < LayerCount; ++i)
	{
		sprintf_s(msg, "\t wid = %.2f \t dc = %.2f", Layers[i].right - Layers[i].left, Layers[i].dc);
		Log(msg);
	}
	Log("\n");

	// Инициализация классов поля
	h = new field(nz, FFTW_ESTIMATE);
	e = new field(nz, FFTW_ESTIMATE);
}

 double Medium::DielCond(int x/*, int t*/) const
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

double Medium::Absorption(int x) const
{
	// gauss
	double rx = realxe(x);
	return AbsorbCoef * exp(-pow((rx - AbsorbCenter) / AbsorbHalfWidth, 10));

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

double Medium::Energy(int x) const
{
	return .5 * (DielCond(x) * e->data[x] * e->data[x] + h->data[x] * h->data[x]);
}

double Medium::ElecEnergy(int x) const
{
	return .5 * DielCond(x) * e->data[x] * e->data[x];
}