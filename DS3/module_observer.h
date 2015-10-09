#include "global.h"

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
public:
	~ObsModule();
	virtual void Init();
	virtual void Tick(int time);
	virtual void PostCalc(int time);
	virtual void Average();

	void AddObserver(int x);
};