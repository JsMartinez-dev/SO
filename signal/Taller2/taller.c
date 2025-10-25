#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int *pidsP;

int mi_indice;
pid_t pidH,pidHH;

void mostrarProceso(){
    printf("Proceso %d - padre [%d]\n",getpid(),getppid());
}
void handler_padre(int sig){
    mostrarProceso(); 
}

void handler_h1(int sig){
    mostrarProceso(); 
    kill(getppid(),SIGUSR1); 
}
void handler_h2(int sig){
    mostrarProceso(); 
    kill(pidH,SIGUSR1);     
}
void handler_h2_2(int sig){
    mostrarProceso(); 
    kill(pidsP[0],SIGUSR1); 
}     
void handler_h1h2(int sig){
    mostrarProceso();
    kill(pidHH,SIGUSR1);
}
void handler_h1h2_2(int sig){
    mostrarProceso(); 
    kill(getppid(),SIGUSR2);
}
void handler_h1h1h2(int sig){
    mostrarProceso();
    kill(getppid(),SIGUSR2);
}
void handler_h3(int sig){
    mostrarProceso(); 
    kill(pidsP[1],SIGUSR1);        
}
void handler_h4(int sig){
    mostrarProceso();
    kill(pidH,SIGUSR1);   
}

void handler_h4_2(int sig){
    mostrarProceso();
    kill(pidsP[2],SIGUSR1);       
}

void handler_h5(int sig){
    mostrarProceso(); 
    kill(pidsP[3],SIGUSR1);
    
}

void handler_h1h4(int sig){
    mostrarProceso(); 
    kill(pidHH,SIGUSR1);    
}
void handler_h1h4_2(int sig){
    mostrarProceso(); 
    kill(getppid(),SIGUSR2); 
}
void handler_h1h1h4(int sig){
    mostrarProceso(); 
    kill(getppid(),SIGUSR2);       
}

int main(){

    signal(SIGUSR1,handler_padre);
    int cantP=5,i;
    pidsP = (int * ) malloc(cantP * sizeof(int));
    
    if(pidsP==NULL){
        perror("FALLO AL RESERVAR MEMORIA");
        return 1;
    }

    for(i=0;i<cantP;i++){
        if((pidsP[i]=fork())==0){
            mi_indice=i;  //Cada proceso hijo guarda su posicion
            break;
        }
    }
    
    if(i<cantP){ //Entran los hijos del padre
        if(i==0){ //hijo 1
            signal(SIGUSR1,handler_h1);
            pause();
            exit(0);
        }
        if(i==1){ // 2do hijo
            signal(SIGUSR1,handler_h2);
            signal(SIGUSR2,handler_h2_2);
            pidH = fork();
            if(pidH==0){ //Entra el hijo 1 del hijo 2
                signal(SIGUSR1,handler_h1h2);
                signal(SIGUSR2,handler_h1h2_2);
                pidHH = fork();
                if(pidHH==0){//Entra el hijo 1 del hijo 1 del hijo 2
                    signal(SIGUSR1,handler_h1h1h2);
                    pause();
                    exit(0);
                }else{
                    for (int i = 0; i < 2; i++)
                    {
                        pause();
                    }
                    exit(0);
                }
            }else{
                for(int j=0;j<2;j++){
                    pause();
                }
                exit(0);
            }
        }     
        if(i==2){// hijo 3
            signal(SIGUSR1,handler_h3);
            pause();
            exit(0);
        }
        if(i==3){// hijo 4
            signal(SIGUSR1,handler_h4);
            signal(SIGUSR2,handler_h4_2);
            pidH = fork();
            if(pidH==0){ //Entra el hijo 1 del hijo 4
                signal(SIGUSR1,handler_h1h4);
                signal(SIGUSR2,handler_h1h4_2);
                pidHH = fork();
                if(pidHH==0){//Entra el hijo 1 del hijo 1 del hijo 4
                    signal(SIGUSR1,handler_h1h1h4);
                    pause();
                    exit(0);
                }else{
                    for (int i = 0; i < 2; i++)
                    {
                        pause();
                    }
                    exit(0);
                }
            }else{
                for(int j=0;j<2;j++){
                    pause();
                }
                    exit(0);
            }
        }
        if(i==4){// hijo 5
            signal(SIGUSR1,handler_h5);
            pause();
            exit(0);
        }   
    }else{ //padre
        printf("--__-INICIO--__-\n");
        printf("Proceso %d - padre [%d]\n",getpid(),getppid());
        sleep(2);
        kill(pidsP[4],SIGUSR1);
        for (int h = 0; h < cantP; h++)
        {
            wait(NULL);
        }
        printf("PADRE FINALIZADO..\n");

    }



    free(pidsP);
    return 0;
}