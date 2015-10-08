#pragma once
#include <list>

using namespace std;

struct DataMapping
{
	std::string String;
	std::string Type;
	void *Ptr;
	char *Default;
};

class Module
{
private:

public:
	virtual void Init() {};
	virtual void Tick(int time) {};
	virtual void PostCalc(int time) {};
};