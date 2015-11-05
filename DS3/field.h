#pragma once
#include "global.h"
#include "../FFTW/fftw3.h"

// Forward decl
double realxe(int i);
double realspec(int i);

class field
{
private:
	int n;
	fftw_complex *sp;
	fftw_plan forward, backward;
	void Dump_Sub(FILE *file, double *data, int n, double (* transform)(int));
public:
	double *data;
	double *spec;
	void Init(int _n, unsigned int flags = FFTW_MEASURE);	// Fourier(CP_INIT);
	void Fourier(bool back = false);
	void Free();
	int GetLen() { return n; }
	void Dump(char *name, double(*transform)(int) = realxe, double(*transformspec)(int) = realspec);
	void DumpFullPrecision(char *name, double(*transform)(int) = realxe, double(*transformspec)(int) = realspec);
	void Dump(FILE *DataFile, FILE *SpecFile, double(*transform)(int) = realxe, double(*transformspec)(int) = realspec);
	void DumpFullPrecision(FILE *DataFile, FILE *SpecFile, double(*transform)(int) = realxe, double(*transformspec)(int) = realspec);
};