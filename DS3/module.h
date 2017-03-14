#pragma once
#include <vector>
class Experiment;

class Module
{
protected:
	Experiment* experiment;

public:
	explicit Module(Experiment* e = nullptr) : experiment(e) {};
	virtual void Init() {};
	virtual void Tick(int time) {};
	virtual void PostCalc(int time) {};
	virtual void Average(std::vector<Module*> modules) {};
};