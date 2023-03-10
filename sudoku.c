#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* validateLine(void *datastruct);
void* validateCol(void *datastruct);
void* validateSub(void *datastruct);

typedef struct inputstruct
{
    int size;
    int a;
    int b;
    int **matrix;
} input;

typedef struct
{
    input *info;
    int i;
    int *invalid;
} datastruct;

int main(int argc, char *argv[])
{
    int n, m, a, b;
    FILE *fp;
    FILE *fOut;

    fp = fopen(argv[1], "r");
    fOut = fopen("sudoku_msap.out", "w");
    if (fp == NULL)
    {
        fprintf(fOut, "Error to read file");
        return -1;
    }

    fscanf(fp, "%dx%d", &n, &m);

    // lembrar de fazer tratamento de erro pro formato do arquivo

    if (n != m)
    {
        fprintf(fOut, "Invalid number of parameters");
        return -1;
    }

    fscanf(fp, "%dx%d", &a, &b);

    if (a * b != m)
    {
        fprintf(fOut, "Invalid number of parameters");
        return -1;
    }

    input info;
    info.a = a;
    info.b = b;
    info.size = n;
    info.matrix = (int **)malloc(n * sizeof(int *)); // reservar numero de linha
    for (int i = 0; i < n; i++)
    {
        info.matrix[i] = (int *)malloc(n * sizeof(int)); // reservar numero de coluna
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &info.matrix[i][j]);
        }
    }
    int invalid = 0;

    for (int i = 0; i < n; i++)
    {
        datastruct data;
        data.info = &info;
        data.i = i;
        data.invalid = &invalid;
        
        validateLine(&data);
        validateCol(&data);
        validateSub(&data);
    }
    if (invalid)
    {
        fprintf(fOut, "FAIL");
        return 1;
    }
    fprintf(fOut, "SUCCESS");
    return 0;
}

void* validateLine(void *vd)
{
    datastruct *ds;
    ds = (datastruct *)vd;
    int mkd[ds->info->size];
    int i = ds->i;

    for (int j = 0; j < ds->info->size; j++)
    {
        mkd[j] = 0;
    }

    for (int j = 0; j < ds->info->size; j++)
    {
        if (ds->info->matrix[i][j] < 1 || ds->info->matrix[i][j] > ds->info->size)
        {
            *ds->invalid = 1; 
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0) // se o numero ja foi usado anteriormente na linha
        {
            *ds->invalid = 1;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}

void * validateCol(void *vd)
{
    datastruct *ds;
    ds = (datastruct *)vd;
    int mkd[ds->info->size];
    int j = ds->i;

    for (int i = 0; i < ds->info->size; i++)
    {
        mkd[i] = 0;
    }

    for (int i = 0; i < ds->info->size; i++)
    {
        if (ds->info->matrix[i][j] < 1 || ds->info->matrix[i][j] > ds->info->size)
        {
            *ds->invalid = 1;
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0)
        {
            *ds->invalid = 1;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}

void* validateSub(void *vd)
{
    datastruct *ds;
    ds = (datastruct *)vd;
    int mkd[ds->info->size];
    int k = ds->i;
    int groupI = k / ds->info->b;
    int groupJ = k % ds->info->b;
    int startI = groupI * ds->info->a; // ponto de partida do grupo
    int startJ = groupJ * ds->info->b;

    for (int i = 0; i < ds->info->size; i++)
    {
        mkd[i] = 0;
    }
    for (int z = 0; z < ds->info->size; z++)
    {
        int i = startI + z % ds->info->a;
        int j = startJ + z / ds->info->b;
        if (ds->info->matrix[i][j] < 1 || ds->info->matrix[i][j] > ds->info->size)
        {
            *ds->invalid = 1;
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0)
        {
            *ds->invalid = 1;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}