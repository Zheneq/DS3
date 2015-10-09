#define _CRT_SECURE_NO_WARNINGS
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <vector>
#include <random>
#include "env.h"
#include "module.h"
#include "field.h"
#include "../Inih/cpp/INIREADER.h"

const double SPEC_LEVEL = .001;

extern INIReader *config;
extern int ExperimentNum;

struct Layer
{
	double left, right, dc;
};

// Forward decl
class EnvInfo;

extern EnvInfo *info;
extern int ExperimentNum, ExperimentCount;

double realxe(int i);
double realxh(int i);
double realte(int i);
double realth(int i);
int idxxe(double x);

inline double DielCond(int x, int t = 0);

double Energy(int x, int t);

double ElecEnergy(int x, int t);

FILE *GetFile(const char *name);