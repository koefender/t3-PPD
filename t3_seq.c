#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//#define DEBUG 1
#define ARRAY_SIZE 1000
#define COL        100000

void bs(int n, int * vetor)
{
    int c=0, d, troca, trocou =1;

    while (c < (n-1) & trocou )
        {
        trocou = 0;
        for (d = 0 ; d < n - c - 1; d++)
            if (vetor[d] > vetor[d+1])
                {
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
                }
        c++;
        }
}

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

int vetor[ARRAY_SIZE][COL];

int main()
{
    int i,j;
    clock_t tempo;

    for (i=0 ; i<ARRAY_SIZE; i++)              /* init array with worst case for sorting */
    {
        for(j=0 ; j<COL; j++)
        {
            vetor[i][j] = COL - 1 - j;
        }
    }

    tempo = clock();

    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)              /* print unsorted array */
        for(j=0 ; j<COL; j++)
            printf("[%03d] ", vetor[i][j]);
    #endif

    for(i=0; i<ARRAY_SIZE; i++)
    {
        //bs(ARRAY_SIZE, vetor);                     /* sort array */
        qsort(vetor[i], COL, sizeof(int), compare);
    }

    #ifdef DEBUG
    printf("\nVetor: ");
    for (i=0 ; i<ARRAY_SIZE; i++)                              /* print sorted array */
        for(j=0 ; j<COL; j++)
            printf("[%03d] ", vetor[i][j]);
    printf("\n");
    #endif

    printf("time:%1.2f\n",(clock()-tempo)/(double)CLOCKS_PER_SEC);

    return 0;
}