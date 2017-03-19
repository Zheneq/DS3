#include "field.h"

// Forward declaration
fftw_plan& GetPlan(int n);

void field::Fourier(bool back)
{
	// return;
	// TODO: Обратное Фурье не из того массива
	if (back)
	{
		throw "Backwards FT is not implemented.";

		/*
		/// Outdated
		if (!bBackward)
		{
			backward = fftw_plan_dft_c2r_1d(n, sp, data, flags);
			bBackward = true;
		}

		fftw_execute(backward);

		////
		for (int i = 0; i < n; i++)
		{
			data[i] /= n;
		}
		////
		//*/
	}
	else
	{
		// TODO: WTH FFTW overwrites the source array?!
		double *temp = fftw_alloc_real(n);
		memcpy(temp, data, n * sizeof(data[0]));

		fftw_execute_dft_r2c(GetPlan(n), temp, sp);

		for (int i = 0; i < n / 2 + 1; i++)
		{
			spec[i] = 2 * (sp[i][0] * sp[i][0] + sp[i][1] * sp[i][1]) / n;
		}
	}
}

field::~field()
{
	fftw_free(sp);
	fftw_free(spec);
	fftw_free(data);
}

/*
void field::Dump(char *name, Medium* medium, double(Medium::*transform)(int), double(Medium::*transformspec)(int))
{
	char fn[256];
	Dump_Sub(GetFile(name), data, n, medium, transform);
	sprintf(fn, "%s-spec", name);
	Dump_Sub(GetFile(fn), spec, n / 2 + 1, medium, transformspec);
}
//*/

void field::Dump(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const, double(Medium::*transformspec)(int) const)
{
	if (DataFile)
		Dump_Sub(DataFile, data, n, medium, transform);
	if (SpecFile)
		Dump_Sub(SpecFile, spec, n / 2 + 1, medium, transformspec);
}

/*
void field::DumpFullPrecision(char *name, Medium* medium, double(Medium::*transform)(int), double(Medium::*transformspec)(int))
{
	char fn[256];
	sprintf(fn, "%s-spec", name);

	FILE *f = GetFile(name), *fs = GetFile(fn);
	DumpFullPrecision(f, fs, medium, transform, transformspec);
	fclose(f);
	fclose(fs);
}
//*/

void field::DumpFullPrecision(FILE *DataFile, FILE *SpecFile, Medium* medium, double(Medium::*transform)(int) const, double(Medium::*transformspec)(int) const)
{
	if (DataFile)
	{
		for (int i = 0; i < n; ++i)
			fprintf(DataFile, "%.10e %.15e\n", (medium->*transform)(i), data[i]);
		fprintf(DataFile, "\n\n");
	}

	if (SpecFile)
	{
		for (int i = 0; i <= n / 2; ++i)
			fprintf(SpecFile, "%.10e %.15e\n", (medium->*transformspec)(i), spec[i]);
		fprintf(SpecFile, "\n\n");
	}
}

void field::Dump_Sub(FILE *file, double *data, int n, Medium* medium, double(Medium::*transform)(int) const)
{
	FILE *fe = file;

	int i;
	fprintf(fe, DumpPattern, (medium->*transform)(0), data[0]);
	// Пропускаем ведущие нули
	for (i = 1; i < n; i++)
	{
		if (abs(data[i]) > eps) break;
	}
	if (i>1) fprintf(fe, DumpPattern, (medium->*transform)(i - 1), 0.0);
	for (; i < n; i++)
	{
		fprintf(fe, DumpPattern, (medium->*transform)(i), data[i]);

		// Если вдруг встретили ноль. Группу последовательных нулей заменяем на две точки по концам отрезка.
		if (abs(data[i]) <= eps)
		{
			int j;
			bool flag = true;
			for (j = i + 1; j < n; ++j)
			{
				if (abs(data[j]) > eps)
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				break;
			}
			else
			{
				if (j - 1 > i) fprintf(fe, DumpPattern, (medium->*transform)(j - 1), 0.0);
				i = j - 1;
			}
		}
	}
	fprintf(fe, DumpPattern, (medium->*transform)(n - 1), data[n - 1]);
	fprintf(fe, "\n\n");

	// fclose(fe);
}