#include "field.h"

void field::Init(int _n, unsigned int flags)
{
	n = _n;

	data = (double*)malloc(sizeof(double)*n);
	sp = fftw_alloc_complex(n / 2 + 1);
	spec = (double*)malloc(sizeof(double)*(n / 2 + 1));

	forward = fftw_plan_dft_r2c_1d(n, data, sp, flags);
	backward = fftw_plan_dft_c2r_1d(n, sp, data, flags);
}

void field::Fourier(bool back)
{
	// TODO: Обратное Фурье не из того массива
	if (back)
	{
		fftw_execute(backward);

		////
		for (int i = 0; i < n; i++)
		{
			data[i] /= n;
		}
		////
	}
	else
	{
		fftw_execute(forward);

		for (int i = 0; i < n / 2 + 1; i++)
		{
			spec[i] = 2 * (sp[i][0] * sp[i][0] + sp[i][1] * sp[i][1]) / n;
		}
	}
}

void field::Free()
{
	fftw_destroy_plan(forward);
	fftw_destroy_plan(backward);
	fftw_free(sp);
	free(spec);
	free(data);
}

void field::Dump(char *name)
{
	char fn[256];
	Dump_Sub(name, data, n);
	sprintf(fn, "%s-spec", name);
	Dump_Sub(fn, spec, n/2 + 1);
}

void field::Dump_Sub(char *name, double *data, int n)
{
	FILE *fe = GetFile(name);

	int i;
	fprintf(fe, info->DumpPattern, realxe(0), data[0]);
	// Пропускаем ведущие нули
	for (i = 1; i < n; i++)
	{
		if (abs(data[i]) > info->eps) break;
	}
	if (i>1) fprintf(fe, info->DumpPattern, realxe(i - 1), 0.0);
	for (; i < n; i++)
	{
		fprintf(fe, info->DumpPattern, realxe(i), data[i]);

		// Если вдруг встретили ноль. Группу последовательных нулей заменяем на две точки по концам отрезка.
		if (abs(data[i]) <= info->eps)
		{
			int j;
			bool flag = true;
			for (j = i + 1; j < n; ++j)
			{
				if (abs(data[j]) > info->eps)
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
				if (j - 1 > i) fprintf(fe, info->DumpPattern, realxe(j - 1), 0.0);
				i = j - 1;
			}
		}
	}
	fprintf(fe, info->DumpPattern, realxe(n - 1), data[n - 1]);
	fprintf(fe, "\n\n");

	fclose(fe);
}