#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define check(c)                      \
    if (validateFormatNumbers(fp, c)) \
        return 1; // pra facilitar a chamada repetitiva



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
    int *invalid;
    int z;
    pthread_mutex_t lock;
} datastruct;

int validateLine(int i, input *info);
int validateCol(int j, input *info);
int validateSub(int k, input *info);
int validateFormat(FILE *fp);
int validateFormatNumbers(FILE *fp, char c);
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

    int num_threads = 12;
    int per_thread = (3 * n + num_threads - 1) / num_threads;
    pthread_t threads[num_threads];
    datastruct data;

    data.info = &info;
    data.invalid = &invalid; // ponteiro que vai passar por referência pra variável fora
    data.z = 0;
    for (int i = 0; i < num_threads; i++)
    {
        int err = pthread_create(&threads[i], NULL, (void *)validate, (void *)&data);
        if (err)
        {
            fprintf(stderr, "Error - pthread_create() return code: %d\n", err);
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < num_threads; i++)
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

        if (mkd[info->matrix[i][j] - 1] != 0) // se o numero ja foi usado anteriormente na linha
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

    // lógica: a linhas tamanho b e b colunas tamanho a
    int groupI = k / info->b; // em que subgrade eu to
    int groupJ = k % info->b;
    int startI = groupI * info->b; // posição do começo do grupo
    int startJ = groupJ * info->a;

    for (int i = 0; i < info->size; i++)
    {
        mkd[i] = 0;
    }
    for (int z = 0; z < info->size; z++)
    {
        int dI = z / info->a; // posição dentro do subgrupo nesse loop
        int dJ = z % info->a;
        int i = startI + dI; // posição atual + começo do grupo pra saber onde to no subgrupo
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

void *validate(void *ptr)
{
    datastruct *ds = (datastruct *)ptr;

    pthread_mutex_lock(&ds->lock);
    int i = ds->z;
    ds->z++;
    pthread_mutex_unlock(&ds->lock);

    while (i < 3 * ds->info->size)
    {
        if (i % 3 == 0)
        {
            *ds->invalid |= validateCol(i / 3, ds->info);
        }
        else if (i % 3 == 1)
        {
            *ds->invalid |= validateLine(i / 3, ds->info);
        }
        else
        {
            *ds->invalid |= validateSub(i / 3, ds->info);
        }
        pthread_mutex_lock(&ds->lock);
        i = ds->z;
        ds->z++;
        pthread_mutex_unlock(&ds->lock);
    }
}