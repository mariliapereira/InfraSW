#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define check(c)                      \
    if (validateFormatNumbers(fp, c)) \
        return 1;

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

int main(int argc, char *argv[])
{
    int n, m, a, b;
    FILE *fp;
    FILE *fOut;

    fp = fopen(argv[1], "r");
    fOut = fopen("sudoku_msap.out", "w");
    if (fp == NULL)
    {
        printf("Error to read file\n");
        return -1;
    }

    if (validateFormat(fp))
    {
        printf("File out of format\n");
        return -1;
    }

    rewind(fp);

    fscanf(fp, "%dx%d", &n, &m);

    if (n != m || n < 1)
    {
        printf("Invalid number of parameters\n");
        return -1;
    }

    fscanf(fp, "%dx%d", &a, &b);

    if (a * b != m)
    {
        printf("Invalid number of parameters\n");
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

    fclose(fp);

    int invalid = 0;

    int num_thread = 12;
    pthread_t threads[num_thread];
    datastruct data[num_thread];
    int per_thread = (n * 3) / num_thread;
    int per_thread_extra = (n * 3) % num_thread;
    int next = 0;

    for (int i = 0; i < num_thread; i++)
    {
        data[i].info = &info;
        data[i].l = next;
        next += per_thread;
        if (i < per_thread_extra)
            next++;
        data[i].r = next;
        data[i].invalid = &invalid;
        int err = pthread_create(&threads[i], NULL, (void *)validate, (void *)&data[i]);
        if (err)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", err);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 3 * n; i++)
    {
        pthread_join(threads[i], NULL);
    }

    if (invalid)
    {
        fprintf(fOut, "FAIL");
        return 1;
    }

    fprintf(fOut, "SUCCESS");
    fclose(fOut);

    for (int i = 0; i < n; i++)
    {
        free(info.matrix[i]);
    }
    free(info.matrix);

    return 0;
}

int validateFormat(FILE *fp)
{
    int n, m;
    char c;

    fscanf(fp, "%d", &n);
    rewind(fp);
    check('x');
    check('\n');
    check('x');
    check('\n');

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n - 1; j++)
        {
            check(' ');
        }
        if (i == n - 1)
        {
            check(EOF);
        }
        else
        {
            check('\n');
        }
    }

    return 0;
}

int validateFormatNumbers(FILE *fp, char c)
{
    int i = 0;
    char x;

    do
    {
        i++;
        x = fgetc(fp);
    } while (x >= '0' && x <= '9');

    if (x != c)
    {
        return 1;
    }

    if (i == 1)
    {
        return 1;
    }

    return 0;
}

int validateLine(int i, input *info)
{
    int mkd[info->size];

    for (int j = 0; j < info->size; j++)
    {
        mkd[j] = 0;
    }

    for (int j = 0; j < info->size; j++)
    {
        if (info->matrix[i][j] < 1 || info->matrix[i][j] > info->size)
        {
            return 1;
        }

        if (mkd[info->matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[info->matrix[i][j] - 1] = 1;
    }
    return 0;
}

int validateCol(int j, input *info)
{
    int mkd[info->size];

    for (int i = 0; i < info->size; i++)
    {
        mkd[i] = 0;
    }

    for (int i = 0; i < info->size; i++)
    {
        if (info->matrix[i][j] < 1 || info->matrix[i][j] > info->size)
        {
            return 1;
        }

        if (mkd[info->matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[info->matrix[i][j] - 1] = 1;
    }
    return 0;
}

int validateSub(int k, input *info)
{
    int mkd[info->size];
    int groupI = k / info->b;
    int groupJ = k % info->b;
    int startI = groupI * info->b;
    int startJ = groupJ * info->a;

    for (int i = 0; i < info->size; i++)
    {
        mkd[i] = 0;
    }
    for (int z = 0; z < info->size; z++)
    {
        int dI = z / info->a;
        int dJ = z % info->a;
        int i = startI + dI;
        int j = startJ + dJ;
        if (info->matrix[i][j] < 1 || info->matrix[i][j] > info->size)
        {
            return 1;
        }

        if (mkd[info->matrix[i][j] - 1] != 0)
        {
            return 1;
        }
        mkd[info->matrix[i][j] - 1] = 1;
    }
    return 0;
}

void *validate(void *vd)
{
    datastruct *ds = (datastruct *)vd;

    for (int i = ds->l; i < ds->r; i++)
    {
        if (i % 3 == 0)
            *ds->invalid += validateLine(i/3, ds->info);
        else if (i % 3 == 1)
            *ds->invalid += validateCol(i/3, ds->info);
        else
            *ds->invalid += validateSub(i/3, ds->info);
    }
}