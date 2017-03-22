#include "module_observer.h"
#include "experiment.h"

// const auto info = experiment->medium;
void ObsModule::Init()
{
	ObsLeft = &AddObserver(0, "left");
	ObsRight = &AddObserver(experiment->medium->nz - 1, "right");

	for (auto rc : RecHeads)
		rc->Init();
	Tick(0);

	init_nrg = 0.0;
	for (int i = 1; i < experiment->medium->nz - 1; ++i)
	{
		init_nrg += pow(experiment->medium->e->data[i], 2);
	}
}

const RecHead& ObsModule::AddObserver(int x, const char* name)
{
	RecHeads.push_back(new RecHead(x, experiment->medium->nt));
	RecHeadNames.push_back(string(name));
	//RecHeads.back()->Init();

	char *msg = new char[256];
	sprintf_s(msg, 256, "Observer %02d (%s) at %f", RecHeads.size() - 1, RecHeadNames.back().c_str(), experiment->medium->realxe(x));
	experiment->Log(msg);
	delete[] msg;

	return *RecHeads.back();
}

const RecHead& ObsModule::GetObserver(const char *name)
{
	const auto item = string(name);
	for (unsigned int i = 0; i < RecHeadNames.size(); ++i)
	{
		if (RecHeadNames[i] == item)
		{
			return *(RecHeads[i]);
		}
	}

	throw "Observer does not exist.";
}

ObsModule::~ObsModule()
{
	for (auto RH : RecHeads)
	{
		if (RH) delete RH;
	}
}

void ObsModule::Tick(int time)
{
	for (auto& RC : RecHeads)
	{
		RC->e->data[time] = experiment->medium->e->data[RC->idx];
		RC->h->data[time] = experiment->medium->h->data[RC->idx];
	}
}

void ObsModule::PostCalc(int time)
{
	// Dump all records into single file
	// TODO: А оно надо?
	FILE *f = experiment->GetFile("records");
	for (auto& RC : RecHeads)
	{
		RC->e->DumpFullPrecision(f, nullptr, experiment->medium, &Medium::realte, nullptr);
	}
	fclose(f);

	left_nrg = 0.0;  right_nrg = 0.0; rest_nrg = 0.0;
	for (int i = 0; i < experiment->medium->nt; ++i)
	{
		left_nrg += pow(ObsLeft->e->data[i], 2);
		right_nrg += pow(ObsRight->e->data[i], 2);
	}
	for (int i = 1; i < experiment->medium->nz - 1; ++i)
	{
		rest_nrg += pow(experiment->medium->e->data[i], 2);
	}

	char msg[256];
	sprintf_s(msg, "Initial  elec energy: %.12e", init_nrg);
	experiment->Log(msg);
	sprintf_s(msg, " <----   elec energy: %.12e", left_nrg);
	experiment->Log(msg);
	sprintf_s(msg, " ---->   elec energy: %.12e", right_nrg);
	experiment->Log(msg);
	sprintf_s(msg, "Residual elec energy: %.12e", rest_nrg);
	experiment->Log(msg);
}

void ObsModule::Average(vector<Module*> modules)
{
	char fn[64];

	field avrg(experiment->medium->nt);

	FILE *fstats = experiment->GetFile("Stats");

	fprintf(fstats, "Coefs for average pulse:\n");

	for (unsigned int i = 0; i < RecHeads.size(); ++i) // Observers
	{
		for (int j = 0; j < experiment->medium->nt; ++j) // Time slices
		{
			double sum = 0;
			for (unsigned int k = 0; k < modules.size(); ++k) // Realizations
			{
				sum += ((ObsModule*)modules[k])->RecHeads[i]->e->data[j];
			}
			avrg.data[j] = sum / modules.size();
		}

		avrg.Fourier();

		sprintf_s(fn, "rec_%s", RecHeadNames[i].c_str());
		FILE *f = experiment->GetFile(fn);
		sprintf_s(fn, "rec_%s-spec", RecHeadNames[i].c_str());
		FILE *fs = experiment->GetFile(fn);

		avrg.DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);

		for (const auto &m : modules)
		{
			((ObsModule*)m)->RecHeads[i]->e->DumpFullPrecision(f, fs, experiment->medium, &Medium::realte, &Medium::realspect);
		}

		if (RecHeadNames[i] == "left" || RecHeadNames[i] == "right")
		{
			double nrg = 0.0f;
			for (int j = 0; j < experiment->medium->nt; ++j) // Time slices
			{
				nrg += pow(avrg.data[j], 2);
			}
			fprintf(fstats, "%s Average:  %.12lf\n", RecHeadNames[i].c_str(), nrg / init_nrg);

		}


		fclose(f);
		fclose(fs);
	}

	double refl_nrg_sum = 0.0, trans_nrg_sum = 0.0, rest_nrg_sum = 0.0;
	for (unsigned int k = 0; k < modules.size(); ++k)
	{
		refl_nrg_sum += ((ObsModule*)modules[k])->left_nrg;
		trans_nrg_sum += ((ObsModule*)modules[k])->right_nrg;
		rest_nrg_sum += ((ObsModule*)modules[k])->rest_nrg;
	}
	double r = refl_nrg_sum / (init_nrg * modules.size());
	double t = trans_nrg_sum / (init_nrg * modules.size());
	double l = rest_nrg_sum / (init_nrg * modules.size());
	double a = 1.0 - r - t - l;

	fprintf(fstats, "Average coefs:\n");
	fprintf(fstats, "Average Refl:  %.12lf\n", r);
	fprintf(fstats, "Average Trans: %.12lf\n", t);
	fprintf(fstats, "Average Abs:   %.12lf\n", a);
	fprintf(fstats, "Average Res:   %.12lf\n", l);

	fclose(fstats);

}