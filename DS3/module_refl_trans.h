#include "global.h"

class RTModule : Module
{
private:
	field *refl, *tran;
public:
	virtual void Init();
	virtual void PostCalc(int time);
};