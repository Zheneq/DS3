#pragma once
#include "module.h"
#include "field.h"
#include <vector>

using namespace std;

struct RecHead
{
	int idx, len;
	field *data;
	vector<field*> Records;

	RecHead(int _idx, int _len)
	{
		idx = _idx;
		len = _len;
		data = NULL;
	}
	void Init()
	{
		data = new field();
		data->Init(len);
	}
	~RecHead()
	{
		if (data) delete data;
		for (auto RS : Records)
		{
			if(RS) delete RS;
		}
	}
	RecHead(const RecHead&) = delete;
	int get_len() { return len; }

};

class ObsModule : public Module
{
	vector<RecHead*> RecHeads;
	vector<string> RecHeadNames;
public:
	explicit ObsModule(Experiment* e = nullptr) : Module(e) {}
	~ObsModule();
	virtual void Init() override;
	virtual void Tick(int time) override;
	virtual void PostCalc(int time) override;
	virtual void Average(vector<Module*> modules) override;

	void AddObserver(int x, const char* name);
};