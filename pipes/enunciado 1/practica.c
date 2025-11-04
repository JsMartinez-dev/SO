#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>




int main() {
    int i;
    char buffer[100];
    char mensaje[100];
    ssize_t bytesLeidos=0;
    pid_t pid = getpid();

    int fd[2];
    int fd2[2];
    int fd3[2];
    int fd4[2];

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
            if(fork()!=0) { //proceso 2
                    close(fd[1]);
                    close(fd2[0]);
                    close(fd3[0]); close(fd3[1]); 
                    close(fd4[0]); close(fd4[1]);
                do{
                    bytesLeidos = read(fd[0], buffer, sizeof(buffer));
                    if(bytesLeidos>0){
                        printf("\nCadena recibida por P2: %s -PID[%d]\n", buffer, getpid());
                        write(fd2[1], buffer, sizeof(buffer));
                    }
                }while(bytesLeidos>0);
                close(fd[0]);
                close(fd2[1]);
                
            } else { //proceso 4
                close(fd[0]); close(fd[1]);
                close(fd2[0]); close(fd2[1]);
                close(fd3[1]); 
                close(fd4[0]);
                do{
                    bytesLeidos = read(fd3[0], buffer, sizeof(buffer));
                    if(bytesLeidos > 0) {
                        printf("\nCadena recibida por P4: %s -PID[%d]\n", buffer, getpid());
                        write(fd4[1], buffer, sizeof(buffer));
                    }
                }while(bytesLeidos>0);
                close(fd3[0]);
                close(fd4[1]);
                

            }
            break;
        
        case 1:
            if(fork()!=0) { // proceso 3
                close(fd[0]); close(fd[1]);
                close(fd2[1]);
                close(fd3[0]);
                close(fd4[0]); close(fd4[1]);
                do{   
                    bytesLeidos = read(fd2[0], buffer, sizeof(buffer));
                    if(bytesLeidos > 0) {
                        printf("\nCadena recibida por P3: %s -PID[%d]\n", buffer, getpid());
                        write(fd3[1], buffer, sizeof(buffer));
                    }
            }while(bytesLeidos>0);
                close(fd2[0]);
                close(fd3[1]);


            } else { // proceso 5
                close(fd[0]); close(fd[1]);
                close(fd2[0]); close(fd2[1]);
                close(fd3[0]); close(fd3[1]);
                close(fd4[1]);
                do{
                    bytesLeidos = read(fd4[0], buffer, sizeof(buffer));
                    if(bytesLeidos > 0){
                        printf("\nCadena recibida por P5: %s -PID[%d]\n\n", buffer, getpid());
                    }
                }while(bytesLeidos>0);
                    close(fd4[0]);

            }
            break;

        default: //padre
            close(fd[0]);
            close(fd2[0]); close(fd2[1]);
            close(fd3[0]); close(fd3[1]);
            close(fd4[0]); close(fd4[1]);

            do{
                printf("Ingresa la cadena: ");
                fgets(mensaje, sizeof(mensaje), stdin);
                mensaje[strcspn(mensaje, "\n")] = '\0';
                printf("\nCadena enviada: %s -PID[%d]\n",mensaje, getpid());

                if(strcmp(mensaje, "fin")) {
                    write(fd[1], mensaje, sizeof(mensaje));
                }
                usleep(5000);

            } while(strcmp(mensaje, "fin"));
            close(fd[1]);
            break;
                          
    }
    
    return 0;
}