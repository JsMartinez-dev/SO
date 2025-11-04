#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int errores(char *msg){
    perror(msg);
    return 1;
}



int main(){


    char *msg = (char *) malloc(100 * sizeof(char));
    char *buff = (char *) malloc(100 * sizeof(char));
    int cantHijos,cantPipes,i,j,k,l;
    int **matriz;
    ssize_t cantBy;

    printf("Indica la cantida de hijos: ");
    scanf("%d",&cantHijos);

    cantPipes=cantHijos+1;
    matriz = (int **) malloc(cantPipes * sizeof(int *));

    for ( i = 0; i < cantPipes; i++)
    {
        matriz[i]= (int *) malloc(2 * sizeof(int));
    }
    

    for ( i = 0; i < cantPipes; i++)
    {
        if(pipe(matriz[i])==-1){
            errores("Error en creacion del pipe");
        }
    }
    


    for ( i = 0; i < cantHijos; i++)
    {
        if(!fork()){

            close(matriz[i][1]);
            read(matriz[i][0],buff,sizeof(buff));
            printf("PID[%d]-MENSAJE :%s\n",getpid(),buff);
            close(matriz[i][0]);
            close(matriz[i+1][0]);      
            cantBy = write(matriz[i+1][1],buff,strlen(buff)+1);
            if(cantBy<0){
                errores("No envio el mensaje el hijo");
            }
            close(matriz[i+1][1]);
            free(msg);
            free(buff);
            exit(0);

        }
    }

    printf("\nDigita el mensaje: ");
    scanf("%s",msg);
    printf("PADRE PID[%d]-MENSAJE :%s\n",getpid(),msg);
    
    
    close(matriz[0][0]);
    cantBy = write(matriz[0][1],msg,strlen(msg)+1);
    if(cantBy<0){
        errores("No envio el mensaje el padre");
    }
    close(matriz[0][1]);
    close(matriz[cantPipes-1][1]);
    read(matriz[cantPipes-1][0],buff,sizeof(buff));
    printf("PADRE PID[%d]-MENSAJE :%s\n",getpid(),buff);
    close(matriz[cantPipes-1][0]);
    

    for ( i = 0; i < cantHijos; i++)
    {
        wait(NULL);
    }
    
    printf("\nPROCESO FINALIZADO\n");

    free(msg);
    free(buff);
    for ( i = 0; i < cantPipes; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
    
    return 0;

}
