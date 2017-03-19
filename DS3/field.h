#pragma once
#include "global.h"
#include "../FFTW/fftw3.h"
#include "medium.h"


class field
{
private:
	int n;
	fftw_complex *sp;
	fftw_plan *forward, *backward;
	unsigned int flags;
	void Dump_Sub(FILE *file, double *data, int n, Medium* medium, double(Medium::*transform)(int) const);
public:

	field(int _n, unsigned int _flags = FFTW_MEASURE) : data(nullptr), forward(nullptr), backward(nullptr), flags(_flags)
	{
		n = _n;

		data = (double*)malloc(sizeof(double)*n);
		sp = fftw_alloc_complex(n / 2 + 1);
		spec = (double*)malloc(sizeof(double)*(n / 2 + 1));
	}

	double *data;
	double *spec;
	void Fourier(bool back = false);
	void Free();
	int GetLen() { return n; }
//	void Dump(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
//	void DumpFullPrecision(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
	void Dump(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
	void DumpFullPrecision(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
};