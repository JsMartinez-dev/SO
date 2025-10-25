#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

/*
<<<<<<< HEAD:signal/NCirculos/ejercicio2.c
Este programa implementa un sistema de comunicación entre procesos
utilizando señales UNIX (SIGUSR1), donde un proceso padre crea
múltiples procesos hijo que se comunican formando un anillo circular
de señales.
=======
Este programa implementa un sistema de comunicación entre procesos utilizando señales UNIX (SIGUSR1),
donde un proceso padre crea múltiples procesos hijo que se comunican formando un anillo circular de señales.
>>>>>>> ccdad3302395b3aec360eaeafb99f4ffa5ff9727:NCirculos/ejercicio2.c
*/

int *pids;
int cantidad;
int mi_indice;

void handler_padre(int sig){

    printf("SOY EL PADRE, RECIBI LA SEÑAL [%d] DE H[1]\n",sig);
    printf("\n PADRE le voy a enviar la señal a mi H[%d]\n-------\n",cantidad);
    kill(pids[cantidad-1],SIGUSR1);
}

void handler_h1(int sig){
    printf("SOY EL HIJO 1, RECIBI LA SEÑAL [%d]\n",sig);

}

void handler_hN(int sig){
   
    int siguiente = mi_indice-1;
    printf("Soy hijo[%d], me llego la señal[%d]\n",mi_indice+1,sig);
    printf("le voy a enviar la señal a hijo[%d]\n ",mi_indice);
    kill(pids[siguiente],SIGUSR1);
            
        
}

void crearHijos(int *pids){
    int i;
    for (i=0; i < cantidad; i++)
    {
        if((pids[i]=fork())==0){
            mi_indice=i;
            break;
        }
    }

    if(i<cantidad){ //hijos
        if(pids[0]==0){ //hijo 1
            signal(SIGUSR1,handler_h1);
            sleep(3);
            printf("\nSOY EL HIJO[%d], le voy a enviar la señal a mi padre [%d]\n",mi_indice+1,getppid());
            kill(getppid(),SIGUSR1);
            pause();
            printf("HIJO 1 FINALIZADO..\n");
            exit(0);
        }else{
            signal(SIGUSR1,handler_hN);
            pause();
            printf("HIJO[%d] finalizado\n",mi_indice+1);
            exit(0);
        }

    }else{
        for (int j = 0; j < cantidad; j++)
        {
            wait(NULL);
        }
        printf("PADRE FINALIZADO\n");
    }

}

int main(int argc, char * argv[]){

    char *archivo = "hijos.txt";
    FILE *fp = fopen(archivo,"r");
    signal(SIGUSR1,handler_padre);
    if(fp==NULL){
        perror("Error al abrir archivo");
        return 1;
    }

    
    if((fscanf(fp,"%d",&cantidad))!=1){
        perror("Error al recibir al hijo");
        fclose(fp);
        return 1;
    }
    
    pids = (int *) malloc(cantidad * sizeof(int));
    
    if(pids==NULL){
        perror("Error al reservar memoria");
        return 0;
    }
    crearHijos(pids);
    
    free(pids);
    fclose(fp);
    return 0;
}
