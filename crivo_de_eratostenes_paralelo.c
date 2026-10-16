#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <mpi.h>

#define MASTER 0

bool *sieve_of_eratosthenes(int *base_primes, int base_limit, int base_count, int start, int len_vector) {
    // Array com os números primos (0 para não primo ou 1 para primo):
    bool *prime_numbers = (bool*) malloc(len_vector * sizeof(bool));

    // Verifica se a memória foi alocada corretamente:
    if(prime_numbers == NULL) {
        printf("A memória não foi alocada corretamente!");
        exit(1);
    }

    // Inicializa o vetor dos números primos com true:
    for(int i = 0; i < len_vector; i++) {
        prime_numbers[i] = true;
    }

    // Verifica se os primos-base foram gerados:
    if(base_primes == NULL) {
        // O processo mestre deve gerar os primos-base:

        // Inicializa 0 e 1 como não primos:
        prime_numbers[0] = false;
        prime_numbers[1] = false;

        for(int i = 2; i <= base_limit; i++) {
            // O número já foi marcado como não primo por outro valor primo menor:
            if(prime_numbers[i] == false) {
                continue;
            }

            // Marca os múltiplos de i como não primos:
            for(int j = i*i; j <= len_vector-1; j += i) {
                prime_numbers[j] = false;
            }
        }
    }

    else {
        // O processo escravo deve gerar os seus primos usando os primos-base:
        
        int first_multiple, prime_number;
        for(int i = 0; i < base_count; i++) {
            prime_number = base_primes[i];
        
            // Identifica qual o primeiro múltiplo de i:
            if((((start + prime_number - 1) / prime_number) * prime_number) > prime_number*prime_number) {
                first_multiple = (((start + prime_number - 1) / prime_number) * prime_number);
            }

            else {
                first_multiple = prime_number*prime_number;
            }

            // Marca os múltiplos de i como não primos:
            for(int j = first_multiple - start; j <= len_vector-1; j += prime_number) {
                prime_numbers[j] = false;
            }
        }
    }

    return prime_numbers;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int n, start, len_vector;
    bool *prime_numbers;
    int base_limit; // valor de raiz de N
    int base_count = 0; // número de primos até raiz de N
    int *base_primes; // números primos até raiz de N 

    // Processo mestre lê o valor de n e determina os primos-base:
    if(rank == MASTER) {
        printf("Valor de n: ");
        scanf("%d", &n);

        // Verifica se o valor de n é válido:
        if(n < 2) {
            printf("O valor %d de n esta invalido!", n);
            exit(1);
        }

        base_limit = (int) sqrt(n);
        // Obtém os primos até raiz de N:
        prime_numbers = sieve_of_eratosthenes(NULL, base_limit, 0, 2, base_limit+1);

        // Conta o número de primos até raiz de N:
        for(int i = 2; i <= base_limit; i++) {
            if(prime_numbers[i]) {
                base_count++;
            }
        }

        base_primes = (int*) malloc(base_count * sizeof(int));
        
        // Verifica se a memória foi alocada corretamente:
        if(base_primes == NULL) {
            printf("A memória não foi alocada corretamente!");
            exit(1);
        }

        // Obtém os números primos até raiz de N:
        int j = 0;
        for(int i = 2; i <= base_limit; i++) {
            if(prime_numbers[i]) {
                base_primes[j] = i;
                j++;
            }
        }
    }

    // Enviando os valores de raiz de N e o total de números primos de 2 até
    // raiz de N:
    MPI_Bcast(&base_limit, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&base_count, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Os processos escravos alocam memória para o array dos primos-base:
    if(rank != MASTER) {
        base_primes = (int*) malloc(base_count * sizeof(int));

        // Verifica se a memória foi alocada corretamente:
        if(base_primes == NULL) {
            printf("A memória não foi alocada corretamente!");
            exit(1);
        }
    }
    
    // Enviando o array dos primos-base para todos os processos:
    MPI_Bcast(base_primes, base_count, MPI_INT, MASTER, MPI_COMM_WORLD);

    // Processo mestre faz a divisão de tarefas:
    if(rank == MASTER) {
        // Dividindo o intervalo de números [2, N] em blocos de tamanho igual e
        // distrbuindo um bloco para cada processo escravo:
        start = 2;
        len_vector = (n-1) / (size-1);
        for(int i = 1; i < size; i++) {    
            // O último escravo recebe o restante dos valores:
            if(i == size-1) {
                len_vector += (n-1) % (size-1);
            }

            MPI_Send(&len_vector, 1, MPI_INT, i, 1, MPI_COMM_WORLD);

            MPI_Send(&start, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            // Incrementa o início do array de cada processo escravo:
            start += len_vector;
        } 
    }


    // Os processos escravos realizam suas tarefas:
    else {
        // Recebendo os valores do array de cada processo escravo:
        MPI_Status status;
        MPI_Recv(&start, 1, MPI_INT, MASTER, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&len_vector, 1, MPI_INT, MASTER, 1, MPI_COMM_WORLD, &status);
        
        // Obtém os primos do array do escravo:
        prime_numbers = sieve_of_eratosthenes(base_primes, base_limit, base_count, start, len_vector);

        int *primes = (int*) malloc(len_vector * sizeof(int));
        
        // Verifica se a memória foi alocada corretamente:
        if(primes == NULL) {
            printf("A memória não foi alocada corretamente!");
            exit(1);
        }

        // Obtém os números primos do array do escravo:
        int j = 0;
        for(int i = 0; i <= len_vector-1; i++) {
            if(prime_numbers[i]) {
                primes[j] = start + i;
                j++;
            }
        }
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