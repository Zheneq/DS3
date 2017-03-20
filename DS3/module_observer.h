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
public:
	ObsModule(const ObsModule&) = delete;
	explicit ObsModule(Experiment* e = nullptr) : Module(e) {}
	~ObsModule();
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
	virtual void Average(vector<Module*> modules) override;

	void AddObserver(int x, const char* name);
	const RecHead& GetObserver(const char *name);
};