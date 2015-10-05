#pragma once
#include "global.h"
#include "../FFTW/fftw3.h"

class field
{
private:
	int n;
	fftw_complex *sp;
	fftw_plan forward, backward;
	void Dump_Sub(char *name, double *data, int n);
public:
	double *data;
	double *spec;
	void Init(int _n, unsigned int flags = FFTW_MEASURE);	// Fourier(CP_INIT);
	void Fourier(bool back = false);
	void Free();
	int GetLen() { return n; }
	void Dump(char *name);
};