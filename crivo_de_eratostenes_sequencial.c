#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

int main(int argc, char *argv[]) {
    int n;

    printf("Valor de n: ");
    scanf("%d", &n);

    // Verifica se o valor de n é válido:
    if(n < 2) {
        printf("O valor %d de n esta invalido!", n);
        exit(1);
    }

    // Array com os números primos (0 para não primo ou 1 para primo):
    bool *prime_numbers = (bool*) malloc((n+1) * sizeof(bool));

    // Verifica se a memória foi alocada corretamente:
    if(prime_numbers == NULL) {
        printf("A memória não foi alocada corretamente!");
        exit(1);
    }

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

    free(prime_numbers);
    fclose(output);

    return 0;
}