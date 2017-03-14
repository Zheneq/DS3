#pragma once
#include "global.h"
#include "../FFTW/fftw3.h"
#include "medium.h"


class field
{
private:
	int n;
	fftw_complex *sp;
	fftw_plan forward, backward;
	void Dump_Sub(FILE *file, double *data, int n, Medium* medium, double(Medium::*transform)(int) const);
public:
	double *data;
	double *spec;
	void Init(int _n, unsigned int flags = FFTW_MEASURE);	// Fourier(CP_INIT);
	void Fourier(bool back = false);
	void Free();
	int GetLen() { return n; }
//	void Dump(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
//	void DumpFullPrecision(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
	void Dump(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
	void DumpFullPrecision(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
};