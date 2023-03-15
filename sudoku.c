#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define check(c)                      \
    if (validateFormatNumbers(fp, c)) \
        return 1; // pra facilitar a chamada repetitiva

int validateFormat(FILE *fp);
int validateFormatNumbers(FILE *fp, char c);
void *validateLine(void *datastruct);
void *validateCol(void *datastruct);
void *validateSub(void *datastruct);

typedef struct inputstruct
{
    int size;
    int a;
    int b;
    int **matrix; // porque não é só um vetor, é bidimensional, tem que ser **
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
        printf("Error to read file\n");
        return -1;
    }

    if (validateFormat(fp))
    {
        printf("File out of format\n");
        return -1;
    }

    rewind(fp); // volta o ponteiro do começo, pra ler os dados agora que validou o formato

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
    info.matrix = (int **)malloc(n * sizeof(int *)); // reservar numero de linha
    for (int i = 0; i < n; i++)
    {
        info.matrix[i] = (int *)malloc(n * sizeof(int)); // reservar numero de coluna
        for (int j = 0; j < n; j++)
        {
            fscanf(fp, "%d", &info.matrix[i][j]);
        }
    }

    fclose(fp);

    int invalid = 0; // variável que vai receber o resultado das threads

    pthread_t threads[3 * n];

    datastruct data[n];

    for (int i = 0; i < n; i++)
    {
        data[i].info = &info;
        data[i].i = i;
        data[i].invalid = &invalid; // ponteiro que vai passar por referência pra variável fora
        int err = pthread_create(&threads[i], NULL, (void *)validateLine, (void *)&data[i]);
        if (err)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", err);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; i++)
    {
        int err = pthread_create(&threads[i + n], NULL, (void *)validateCol, (void *)&data[i]);
        if (err)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", err);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < n; i++)
    {
        int err = pthread_create(&threads[i + 2*n], NULL, (void *)validateSub, (void *)&data[i]);
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

    if (x != c) // se o caracter for diferente do passado na chamada
    {
        return 1;
    }

    if (i == 1) // se o arquivo só tiver um caracter é inválido
    {
        return 1;
    }

    return 0;
}

void *validateLine(void *vd)
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
            break;
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0) // se o numero ja foi usado anteriormente na linha
        {
            *ds->invalid = 1;
            break;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}

void *validateCol(void *vd)
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
            break;
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0)
        {
            *ds->invalid = 1;
            break;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}

void *validateSub(void *vd)
{
    datastruct *ds;
    ds = (datastruct *)vd;
    int mkd[ds->info->size];
    int k = ds->i;
    // lógica: a linhas tamanho b e b colunas tamanho a
    int groupI = k / ds->info->b; // em que subgrade eu to
    int groupJ = k % ds->info->b;
    int startI = groupI * ds->info->b; // posição do começo do grupo
    int startJ = groupJ * ds->info->a;

    for (int i = 0; i < ds->info->size; i++)
    {
        mkd[i] = 0;
    }
    for (int z = 0; z < ds->info->size; z++)
    {
        int dI = z / ds->info->a; // posição dentro do subgrupo nesse loop
        int dJ = z % ds->info->a;
        int i = startI + dI; // posição atual + começo do grupo pra saber onde to no subgrupo
        int j = startJ + dJ;
        if (ds->info->matrix[i][j] < 1 || ds->info->matrix[i][j] > ds->info->size)
        {
            *ds->invalid = 1;
            break;
        }

        if (mkd[ds->info->matrix[i][j] - 1] != 0)
        {
            *ds->invalid = 1;
            break;
        }
        mkd[ds->info->matrix[i][j] - 1] = 1;
    }
}