#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *fp;
    int filas, columnas;
    int **matriz;

    // 1️⃣ Abrir archivo
    fp = fopen("matriz.txt", "r");
    if (fp == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    // 2️⃣ Leer dimensiones
    fscanf(fp, "%d %d", &filas, &columnas);

    // 3️⃣ Reservar memoria dinámica
    matriz = (int **)malloc(filas * sizeof(int *));
    for (int i = 0; i < filas; i++) {
        matriz[i] = (int *)malloc(columnas * sizeof(int));
    }

    // 4️⃣ Leer los datos de la matriz
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            fscanf(fp, "%d", &matriz[i][j]);
        }
    }

    // 5️⃣ Mostrar la matriz
    printf("Matriz leída del archivo:\n");
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%d ", matriz[i][j]);
        }
        printf("\n");
    }

    // 6️⃣ Liberar memoria
    for (int i = 0; i < filas; i++) {
        free(matriz[i]);
    }
    free(matriz);

    // 7️⃣ Cerrar archivo
    fclose(fp);

    return 0;
}
