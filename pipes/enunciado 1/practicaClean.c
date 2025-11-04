#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void mostrar(int *fd1, int num_proceso, int *fd2){
    char buffer[100];
    ssize_t bytesLeidos;
    
    do{
        bytesLeidos = read(fd1[0], buffer, sizeof(buffer));
        if(bytesLeidos > 0){
            printf("\nCadena recibida por P%d: %s -PID[%d]\n", num_proceso, buffer, getpid());
        
            if(strcmp(buffer, "fin") == 0) {
                write(fd2[1], buffer, sizeof(buffer));
                break;
            }
            
            write(fd2[1], buffer, sizeof(buffer));
        }               
    } while(bytesLeidos > 0);
    
    close(fd1[0]);
    close(fd2[1]);
}

int main() {
    int i;
    char mensaje[100];
    
    int fd[2];   // P1 -> P2
    int fd2[2];  // P2 -> P3
    int fd3[2];  // P3 -> P4
    int fd4[2];  // P4 -> P5

    pipe(fd);
    pipe(fd2);
    pipe(fd3);
    pipe(fd4);

    for (i = 0; i < 2; i++)
    {
        if(!fork()) break;
    }

    switch (i) {
        case 0:
            if(fork() != 0) { // Proceso 2
                close(fd[1]);
                close(fd2[0]);
                close(fd3[0]); close(fd3[1]); 
                close(fd4[0]); close(fd4[1]); 
                mostrar(fd, 2, fd2);
                
            } else { // Proceso 4
                close(fd[0]); close(fd[1]);
                close(fd2[0]); close(fd2[1]);
                close(fd3[1]); 
                close(fd4[0]);         
                mostrar(fd3, 4, fd4);
            }
            break;
            
        case 1:
            if(fork() != 0) { // Proceso 3
                close(fd[0]); close(fd[1]);
                close(fd2[1]);
                close(fd3[0]);
                close(fd4[0]); close(fd4[1]);          
                mostrar(fd2, 3, fd3);
                
            } else { // Proceso 5
                close(fd[0]); close(fd[1]);
                close(fd2[0]); close(fd2[1]);
                close(fd3[0]); close(fd3[1]);
                close(fd4[1]);
                
                char buffer[100];
                ssize_t bytesLeidos;
                
                do{
                    bytesLeidos = read(fd4[0], buffer, sizeof(buffer));
                    if(bytesLeidos > 0){
                        printf("\nCadena recibida por P5: %s -PID[%d]\n", buffer, getpid());
                        
                        // Si recibimos "fin", terminamos
                        if(strcmp(buffer, "fin") == 0) {
                            break;
                        }
                    }
                } while(bytesLeidos > 0);
                
                close(fd4[0]);
            }
            break;

        default: // Proceso padre (P1)
            close(fd[0]);
            close(fd2[0]); close(fd2[1]);
            close(fd3[0]); close(fd3[1]);
            close(fd4[0]); close(fd4[1]);

            do{
                printf("Ingresa la cadena: ");
                fgets(mensaje, sizeof(mensaje), stdin);
                mensaje[strcspn(mensaje, "\n")] = '\0';
                
                printf("\nCadena enviada: %s -PID[%d]\n", mensaje, getpid());            
                write(fd[1], mensaje, sizeof(mensaje));
                usleep(5000);

            } while(strcmp(mensaje, "fin") != 0); // Continúa mientras NO sea "fin"
            
            close(fd[1]);
            
            for(int j = 0; j < 4; j++) {
                wait(NULL);
            }
            
            printf("\nTodos los procesos han terminado.\n");
            break;
    }
    
    return 0;
}