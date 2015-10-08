#include "global.h"

class RTModule : public Module
{
private:
	field *refl, *tran;
public:
	RTModule();
	~RTModule();
	virtual void Init();
	virtual void PostCalc(int time);
	void Dump();
};