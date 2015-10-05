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
	Module();
	~Module();
	virtual void Init() = 0;
	virtual void Tick(int time) = 0;
	virtual void PostCalc(int time) = 0;
};