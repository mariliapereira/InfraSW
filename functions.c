#include "functions.h"
#define check(c)                      \
    if (validateFormatNumbers(fp, c, n)) \
        return 1;

int validateFormat(FILE *fp)
{
    int n, m;

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
            check(' '); //conferindo espaços
        }
        if (i == n - 1)
        {
            check(EOF); //a última linha não tem espaço nem \n, é o fim do arquivo
        }
        else
        {
            check('\n'); //toda linha (menos a última) termina em \n
        }
    }

    return 0;
}

int validateFormatNumbers(FILE *fp, char c, int n)
{
    int i = 0, z = 0;
    char x;

    do
    {
        i++;
        x = fgetc(fp);
        if (x >= '0' && x <= '9')
        {
            z = z * 10 + x - '0';
        }
    } while (x >= '0' && x <= '9');

    if (z > n || z < 1) //caso o número seja maior que N
    {
        return 1;
    }
    if (x != c)
    {
        return 1;
    }

    if (i == 1) //garantia de que tem pelo menos 1 número
    {
        return 1;
    }

    if (i > 8) //prevenção de Overflow
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
    return NULL;
}