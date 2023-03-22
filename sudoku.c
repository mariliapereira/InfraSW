#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "functions.h"


int main(int argc, char *argv[])
{
    int n, m, a, b;
    FILE *fp;
    FILE *fOut;

    if (argc != 2)
    {
        printf("Invalid number of parameters\n"); //garante o número certo de argumentos
        return -1;
    }

    fp = fopen(argv[1], "r");
    fOut = fopen("sudoku_msap.out", "w");
    if (fp == NULL) //caso o arquivo não seja possível de ser aberto
    {
        printf("Error to read file\n");
        return -1;
    }

    if (validateFormat(fp))
    {
        printf("File out of format\n"); //detecta char errado ou número inválido
        return -1;
    }

    rewind(fp);

    fscanf(fp, "%dx%d", &n, &m);
    if (n != m || n < 1)
    {
        printf("File out of format\n"); //se não for uma NxN
        return -1;
    }

    fscanf(fp, "%dx%d", &a, &b);

    if (a * b != m)
    {
        printf("File out of format\n"); //conferindo a validade dos argumentos das subgrades
        return -1;
    }

    input info;
    info.a = a;
    info.b = b;
    info.size = n;
    info.matrix = (int **)malloc(n * sizeof(int *));
    if(info.matrix == NULL)
    {
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++)
    {
        info.matrix[i] = (int *)malloc(n * sizeof(int));
        if(info.matrix[i] == NULL)
        {
            exit(EXIT_FAILURE);
        }

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

    for (int i = 0; i < num_thread; i++)
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