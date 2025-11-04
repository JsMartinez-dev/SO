#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>

pid_t pidHijos[2];
pid_t pidHijos2[2];
pid_t pidH;
int nVueltas,vueltasRestantes;

void handler_padre(int sig) {
    vueltasRestantes--;
    printf("Vueltas restantes: %d\n",vueltasRestantes);
    if(vueltasRestantes>0){
        printf("padre [%d]\n", getpid());
        kill(pidHijos[1],SIGUSR1);
        usleep(1000);
    }else{
        printf("\nPROCESO FINALIZADO\n");
        exit(0);
    }
}

void handler_H1(int sig) {
    printf("H1 [%d]\n", getpid());
    kill(pidHijos2[1],SIGUSR1);
}

void handler_H1_2(int sig){
    printf("H1 [%d]\n", getpid());
    kill(pidHijos2[0],SIGUSR1);
}
void handler_H1_3(int sig){
    printf("H1 [%d]\n", getpid());
    kill(getppid(),SIGUSR1);
    usleep(3000);
}

void handler_H11 (int sig) {
    printf("H11 [%d]\n", getpid());
    kill(pidH,SIGUSR1);
}
void handler_H11_2(int sig){
    //Segunda mano de h11
    printf("H11 [%d]\n", getpid());
    kill(getppid(),SIGPWR);
}

void handler_H12(int sig) {
    printf("H12 [%d]\n", getpid());
    kill(getppid(),SIGUSR2);
}

void handler_H112(int sig) {
    printf("H112 [%d]\n", getpid());
    kill(getppid(),SIGUSR2);
}

void handler_H2(int sig) {
    printf("H2 [%d]\n",getpid());
    kill(pidHijos2[1],SIGUSR1);

}
void handler_H2_2(int sig){
    printf("H2 [%d]\n", getpid());
    kill(pidHijos2[0],SIGUSR1);
    
}

void handler_H2_3(int sig){
    printf("H2 [%d]\n", getpid());
    kill(pidHijos[0], SIGUSR1);
}
void handler_H21(int sig) {
    printf("H21 [%d]\n", getpid());
    kill(getppid(),SIGPWR);
}

void handler_H22(int sig) {
    printf("H22 [%d]\n", getpid());
    kill(getppid(),SIGUSR2);
}

void esperar(int vueltas){
    for (int j = 0; j < vueltas-1; j++)
    {
        pause();
    }
}


int main() {

    int i,j,k,l;
    
    printf("Digite el numero de vueltas: ");
    scanf("%d",&nVueltas); //4<-  24234<-&

    signal(SIGUSR1,handler_padre);
    vueltasRestantes=nVueltas;

    pid_t getpid(), getppid();
    
    for (i = 0; i < 2; i++) {
        if ((pidHijos[i]=fork())==0) break;     
    }
    
    switch (i)
    {
    case 0: // H1
        signal(SIGUSR1,handler_H1);
        signal(SIGUSR2,handler_H1_2);
        signal(SIGPWR,handler_H1_3);
        
        for ( j = 0; j < 2; j++)
        {
            if ((pidHijos2[j]=fork())==0)
            {
                if(j==0){ //H11
                    signal(SIGUSR1,handler_H11);
                    signal(SIGUSR2,handler_H11_2);
                    pidH = fork();
                    if(pidH == 0){ //H112
                        signal(SIGUSR1,handler_H112);    
                        esperar(nVueltas);
            
                    }
                    esperar(nVueltas);
                    wait(NULL);
        
                }else{ //H12
                    signal(SIGUSR1,handler_H12);   
                    esperar(nVueltas);
                    }
                }
            }
            esperar(nVueltas);
            wait(NULL);
            wait(NULL);
        break; 

    case 1: // H2
    
        signal(SIGUSR1,handler_H2);  
        signal(SIGUSR2, handler_H2_2);
        signal(SIGPWR, handler_H2_3); 
        
        for (k = 0; k < 2; k++) {
            if ((pidHijos2[k] = fork()) == 0){
                if(k == 0){ //H21
                    signal(SIGUSR1, handler_H21);
                    esperar(nVueltas);
                } else { // H22
                    signal(SIGUSR1,handler_H22);   
                    esperar(nVueltas);
                }
            }    
        }
        esperar(nVueltas);
        wait(NULL);
        wait(NULL);
        break;
    
    default: //padre
        
        printf("padre [%d]\n",getpid());
        sleep(2);
        kill(pidHijos[1],SIGUSR1);
        wait(NULL);
        wait(NULL);       
        break;
    }
    
    return 0;

}