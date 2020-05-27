#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define COL 100000 + 1 // Numero de colunas + 1 é pro espaço pra enviar a linha
#define LIN 1000 // Numero de linhas

enum {
  REQUEST_TAG  = 0,
  RESPONSE_TAG = 1,
  SUICIDE_TAG = 2
};

void bs(int n, int * vetor) {
  int c=0, d, troca, trocou =1;

  while (c < (n-1) & trocou ) {
    trocou = 0;
    for (d = 0 ; d < n - c - 1; d++)
      if (vetor[d] > vetor[d+1]) {
        troca = vetor[d];
        vetor[d] = vetor[d+1];
        vetor[d+1] = troca;
        trocou = 1;
      }
      c++;
  }
}

int compare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

main(int argc, char** argv) {
  /** MPI */
  int my_rank;  // Identificador deste processo
  int proc_n;   // Numero de processos disparados pelo usuário na linha de comando (np)

  MPI_Status status;
  MPI_Init(&argc, &argv); // Funcao que inicializa o MPI, todo o código paralelo esta abaixo
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); // Pega o numero do processo atual (rank)
  MPI_Comm_size(MPI_COMM_WORLD, &proc_n); // Pega informação do numero de processos (quantidade total)

  int (*saco_de_tarefas)[COL] = malloc (LIN * sizeof * saco_de_tarefas);
  int *vetor_tarefa = (int *)malloc(sizeof(int)*COL);
  int i, j; 

  if ( my_rank != 0 ) { // Sou escravo
   while(1) {     
      MPI_Send(&my_rank, 1, MPI_INT, 0, REQUEST_TAG, MPI_COMM_WORLD); // Solicita tarefa para o mestre(rank 0)

      MPI_Recv(&vetor_tarefa[0], COL, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); // Recebo o vetor do mestre

      if (status.MPI_TAG == SUICIDE_TAG) break; // Se não tiver mais vetor para ordenar, finalizo

      // Processa
      // bs(COL - 1, vetor_tarefa);
      qsort (vetor_tarefa, COL - 1, sizeof(int), compare);
      
      MPI_Send(&vetor_tarefa[0], COL, MPI_INT, 0, RESPONSE_TAG, MPI_COMM_WORLD); // Retorna o vetor ordenado para o mestre
    }
  } else { // Sou mestre
    // Preenche matriz
    for(i = 0; i < LIN; i++) {
    	for(j = 0; j < COL - 1; j++) 
        saco_de_tarefas[i][j] = COL-j;
    }
    
    double time = MPI_Wtime();
    int linha = 0;
    int deadProcess = 0;

    while(1) {
      // Recebo alguma mensagme do escravo, pode ser request ou response
      MPI_Recv(&vetor_tarefa[0], COL, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 

      if (status.MPI_TAG == RESPONSE_TAG) { // Se for resposta do escravo com vetor ordenado, guarda o vetor
        for(j = 0; j < COL - 1; j++)  saco_de_tarefas[vetor_tarefa[COL-1]][j] = vetor_tarefa[j]; 
      } else if (linha >= LIN) { // Se for uma request mas acabou os vetores, manda suicide
        MPI_Send(&vetor_tarefa[0], 1, MPI_INT, vetor_tarefa[0], SUICIDE_TAG, MPI_COMM_WORLD); // Aqui o importante é enviar a tag

        deadProcess++;
        if(deadProcess >= proc_n - 1) { // Se todos os processos morreram, finalizo
          /*printf("===================\n");
          for(i = 0; i < LIN; i++) {
            printf("\nLinha %d: ", i);
    	      for(j = 0; j < COL - 1; j++) {
              printf("%d ", saco_de_tarefas[i][j]);
              // printf("linha: %d, coluna: %d, valor: %d\n", i, j, saco_de_tarefas[i][j]);
            }
          } */
          printf("time: %1.2f\n", MPI_Wtime() - time);
          break;
        }
      } else { // Envia vetor
        saco_de_tarefas[linha][COL-1] = linha; // Guarda o número da linha no último espaço reservado
        MPI_Send(&saco_de_tarefas[linha], COL, MPI_INT, vetor_tarefa[0], RESPONSE_TAG, MPI_COMM_WORLD);
        linha++;
      }
    }
  }
  
  free(saco_de_tarefas);
  free(vetor_tarefa);
  MPI_Finalize();
}
