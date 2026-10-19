#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

void sieve_of_eratosthenes(bool *prime_numbers, int n) {
    prime_numbers[0] = false;
    prime_numbers[1] = false;

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
    }
}

int main(int argc, char *argv[]) {
    // Verifica se os argumentos são válidos:
    if(argc != 2) {
        printf("Argumentos inválidos!\n");
        printf("Formato correto: ./crivo_de_eratostenes_sequencial N\n");
        exit(1);
    }

    int n = atoi(argv[1]);

    // Verifica se o valor de n é válido:
    if(n < 2) {
        printf("O valor %d de n esta inválido!", n);
        exit(1);
    }

    // Array com os números primos (0 para não primo ou 1 para primo):
    bool *prime_numbers = (bool*) malloc((n+1) * sizeof(bool));

    // Verifica se a memória foi alocada corretamente:
    if(prime_numbers == NULL) {
        printf("A memória não foi alocada corretamente!");
        exit(1);
    }

    sieve_of_eratosthenes(prime_numbers, n);

    FILE *output = fopen("primos.txt", "w");

    // Verifica se o arquivo de saída foi criado corretamente:
    if(output == NULL) {
        printf("Ocorreu um erro ao criar o arquivo de saída \"primos.txt\"!");
        exit(1);
    }

    // Salvando os números primos no arquivo de saída:
    for(int i = 2; i <= n; i++) {
        if(prime_numbers[i]) {
            fprintf(output, "%d\n", i);
        }
    }

    fclose(output);
    
    free(prime_numbers);

    return 0;
}