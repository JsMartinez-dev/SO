#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int nHIjos, i, j, p;
    char msg[100];
    char buff[100];
    ssize_t cantBytes;

    printf("Indica el numero de hijos: ");
    scanf("%d", &nHIjos);

    printf("Indica el mensaje a enviar: ");
    scanf("%s", msg);

    int cantPIpes = nHIjos * 2;
    int matriz[cantPIpes][2];

    // Crear los pipes
    for (j = 0; j < cantPIpes; j++) {
        if (pipe(matriz[j]) == -1) {
            perror("Error al crear el pipe");
            exit(1);
        }
    }

    // Crear los hijos
    for (i = 0; i < nHIjos; i++) {
        if (!fork())
            break;
    }

    // Proceso padre
    if (i >= nHIjos) {   //Cambiel == por >= para mayor seguridad
        for (p = 0; p < cantPIpes; p++) {
            if (p == 0) {
                close(matriz[p][0]);
            } else if (p == cantPIpes - 1) {
                close(matriz[p][1]);
            } else {
                close(matriz[p][0]);
                close(matriz[p][1]);
            }
        }

        printf("\nPID[%d] El padre envió el mensaje %s\n", getpid(), msg);

        cantBytes = write(matriz[0][1], msg, strlen(msg) + 1);
        if (cantBytes < 0) {
            perror("Error de escritura en el padre");
        }
        close(matriz[0][1]);

        cantBytes = read(matriz[cantPIpes - 1][0], buff, sizeof(buff));
        if (cantBytes > 0) {
            printf("PID[%d] El padre recibió el mensaje %s de regreso\n", getpid(), buff);
        } //Quite el else para que el padre no quede esperando mas mensajes

        close(matriz[cantPIpes - 1][0]);

        for (i = 0; i < nHIjos; i++) {
            wait(NULL);
        }
    }else{
 // Ida
            cantBytes = read(matriz[i][0], buff, sizeof(buff));
            if (cantBytes > 0) {
                printf("PID[%d] El hijo recibió el mensaje %s de ida\n", getpid(), buff);
            }
            close(matriz[i][0]);

            write(matriz[i + 1][1], buff, strlen(buff) + 1);
            close(matriz[i + 1][1]);

            // Regreso
            cantBytes = read(matriz[cantPIpes - 2 - i][0], buff, sizeof(buff)); //Cambie 1 por 2
            if (cantBytes > 0) {
                printf("PID[%d] El hijo recibió el mensaje %s de regreso\n", getpid(), buff);
            }
            close(matriz[cantPIpes - 2 - i][0]);

                write(matriz[cantPIpes - 1 - i][1], buff, strlen(buff) + 1); //Cambie 2 por 1   
                close(matriz[cantPIpes - 1 - i][1]);
            
            exit(0);
    }

    return 0;
    }