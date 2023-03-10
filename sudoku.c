#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct inputstruct
{
    int size;
    int a;
    int b;
    int **matrix;
} input;

int validateLine(int i, input in);
int validateCol(int j, input in);
int validateSub(int k, input in);

int main(int argc, char* argv[])
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
    info.matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        info.matrix[i] = (int *)malloc(n * sizeof(int));
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &info.matrix[i][j]);
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (validateLine(i, info))
        {
            fprintf(fOut, "FAIL");
            return 1;
        }
        if (validateCol(i, info))
        {
            fprintf(fOut, "FAIL");
            return 1;
        }
        if (validateSub(i, info))
        {
            fprintf(fOut, "FAIL");
            return 1;
        }
    }

    fprintf(fOut, "SUCCESS");
    return 0;
}

int validateLine(int i, input in)
{
    int mkd[in.size];

    for (int j = 0; j < in.size; j++)
    {
        mkd[j] = 0;
    }

    for (int j = 0; j < in.size; j++)
    {
        if (in.matrix[i][j] < 1 || in.matrix[i][j] > in.size)
        {
            return 1;
        }

        if (mkd[in.matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[in.matrix[i][j] - 1] = 1;
    }

    return 0;
}

int validateCol(int j, input in)
{
    int mkd[in.size];

    for (int i = 0; i < in.size; i++)
    {
        mkd[i] = 0;
    }

    for (int i = 0; i < in.size; i++)
    {
        if (in.matrix[i][j] < 1 || in.matrix[i][j] > in.size)
        {
            return 1;
        }

        if (mkd[in.matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[in.matrix[i][j] - 1] = 1;
    }

    return 0;
}

int validateSub(int k, input info)
{

    int mkd[info.size];
    int groupI = k / info.b;
    int groupJ = k % info.b;
    int startI = groupI * info.a;
    int startJ = groupJ * info.b;

    for (int i = 0; i < info.size; i++)
    {
        mkd[i] = 0;
    }
    for (int z = 0; z < info.size; z++)
    {
        int i = startI + z % info.a;
        int j = startJ + z / info.b;
        if (info.matrix[i][j] < 1 || info.matrix[i][j] > info.size)
        {
            return 1;
        }

        if (mkd[info.matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[info.matrix[i][j] - 1] = 1;
    }
    return 0;
}