#pragma once
#include "global.h"
#include "../FFTW/fftw3.h"
#include "medium.h"


class field
{
private:
	int n;
	fftw_complex *sp;
	bool bForward, bBackward;
	unsigned int flags;
	void Dump_Sub(FILE *file, double *data, int n, Medium* medium, double(Medium::*transform)(int) const);
public:

	field(int _n, unsigned int _flags = FFTW_MEASURE) : data(nullptr), bForward(false), bBackward(false), flags(_flags)
	{
		n = _n;

		data = (double*)fftw_alloc_real(n);
		sp = fftw_alloc_complex(n / 2 + 1);
		spec = (double*)fftw_alloc_real(n / 2 + 1);
	}
	field(const field &other) : field(other.n)
	{
		memcpy(data, other.data, sizeof(data[0]) * n);
		memcpy(sp, other.sp, sizeof(sp[0]) * (n / 2 + 1));
		memcpy(spec, other.spec, sizeof(spec[0]) * (n / 2 + 1));
	}
	~field();

	double *data;
	double *spec;
	void Fourier(bool back = false);
	int GetLen() { return n; }
//	void Dump(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
//	void DumpFullPrecision(char *name, Medium* medium, double(Medium::*transform)(int) = &Medium::realxe, double(Medium::*transformspec)(int) = &Medium::realspec);
	void Dump(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
	void DumpFullPrecision(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const = &Medium::realxe, double(Medium::*transformspec)(int) const = &Medium::realspec);
};