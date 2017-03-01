#include "global.h"

class RTModule : public Module
{
private:
	field *refl, *tran;
public:
	RTModule();
	~RTModule();
	virtual void Init() override;
	virtual void PostCalc(int time) override;
	void Dump();
};