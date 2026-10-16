#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n, base_primes, start, len_vector;
    
    // Processo mestre:
    if(rank == MASTER) {
        printf("Valor de n: ");
        scanf("%d", &n);

        // Verifica se o valor de n é válido:
        if(n < 2) {
            printf("O valor %d de n esta invalido!", n);
            exit(1);
        }

        // Dividindo o intervalo de números [2, N] em blocos de tamanho igual e
        // distrbuindo um bloco para cada processo escravo:
        start = 2;
        len_vector = (n-1) / (size-1);
        for(int i = 1; i < size; i++) {    
            // O último escravo recebe o restante dos valores:
            if(i == size-1) {
                len_vector += (n-1) % (size-1);
            }

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&len_vector, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            // Incrementa o início do vetor de cada process escravo:
            start += len_vector;
        }

        base_primes = (int) sqrt(n);
        
        // Enviando os primos-base para todos os processos:
        MPI_Bcast(&base_primes, 1, MPI_INT, MASTER, MPI_COMM_WORLD);        
    }

    else {
        // Processos escravos:
        // Recebendo os valores do vetor de cada processo escravo:
        MPI_Status status;
        MPI_Recv(&start, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&len_vector, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &status);

        // Array com os números primos (0 para não primo ou 1 para primo):
        bool *prime_numbers = (bool*) malloc(len_vector * sizeof(bool));

        // Verifica se a memória foi alocada corretamente:
        if(prime_numbers == NULL) {
            printf("A memória não foi alocada corretamente!");
            exit(1);
        }

        // Inicializa o vetor dos números primos com true:
        for(int i = 2; i <= n; i++) {
            prime_numbers[i] = true;
        }

        for(int i = 2; i <= (int) sqrt(n); i++) {
        // O número já foi marcado como não primo por outro valor primo menor:
        if(prime_numbers[i] == false) {
            continue;
        }

        // Marca os múltiplos de i como não primos:
        for(int j = i*i; j <= n; j += i) {
            prime_numbers[j] = false;
        }
        free(prime_numbers);
    }

    // O processo mestre salva os números primos no arquivo de saída:
    if(rank == MASTER) {
        FILE *output = fopen("primos.txt", "w");

        // Verifica se o arquivo de saída foi criado corretamente:
        if(output == NULL) {
            printf("Ocorreu um erro ao criar o arquivo de saída \"primos.txt\"!");
            exit(1);
        }

        for(int i = 2; i <= n; i++) {
            if(prime_numbers[i]) {
                fprintf(output, "%d\n", i);
            }
        }
        fclose(output);
    }
    
    MPI_Finalize();

    return 0;
}
