#pragma once
#include "module.h"
#include "field.h"
#include <vector>

using namespace std;

struct RecHead
{
	int idx, len;
	field *e, *h;

	RecHead(int _idx, int _len) : idx(_idx), len(_len), e(nullptr), h(nullptr) {}

	void Init()
	{
		e = new field(len);
		h = new field(len);
	}
	~RecHead()
	{
		if (e) delete e;
		if (h) delete h;
	}
	RecHead(const RecHead&) = delete;
	int get_len() { return len; }

};

class ObsModule : public Module
{
	vector<RecHead*> RecHeads;
	vector<string> RecHeadNames;
	const RecHead *ObsLeft, *ObsRight;

	// TODO: Move this to another class
	// elec energy passed the left/right boundary, residual one and initial one
	double left_nrg, right_nrg, rest_nrg, init_nrg;
public:
	ObsModule(const ObsModule&) = delete;
	explicit ObsModule(Experiment* e = nullptr) : Module(e) {}
	~ObsModule();
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
	virtual void Average(vector<Module*> modules) override;

	const RecHead& AddObserver(int x, const char* name);
	const RecHead& GetObserver(const char *name);
};