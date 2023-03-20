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

typedef struct
{
    input *info;
    int l, r;
    int *invalid;
} datastruct;

int validateFormat(FILE *fp);
int validateFormatNumbers(FILE *fp, char c);
int validateLine(int i, input *info);
int validateCol(int j, input *info);
int validateSub(int k, input *info);
void *validate(void *datastruct);