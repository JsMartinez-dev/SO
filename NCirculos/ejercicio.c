#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


int pids[3];

void handler_padre(int sig){

    printf("SOY EL PADRE, RECIBI LA SEÑAL [%d] DE H1",sig);

    //Enviamos la señal al hijo3, desde el padre
    printf("\nSoy el PADRE, le voy a enviar la señal a mi H3 [%d]\n-------\n",pids[2]);
    kill(pids[2],SIGUSR1);
}
void handler_h3(int sig){
    printf("SOY EL HIJO 3, RECIBI LA SEÑAL [%d] DE PADRE",sig);
    //Enviamos la señal al hijo 2, desde el hijo 3
    printf("\nSoy el HIJO 3, le voy a enviar la señal al H2 [%d]\n-------\n",pids[1]);
    kill(pids[1],SIGUSR1);
}
void handler_h2(int sig){
    printf("SOY EL HIJO 2, RECIBI LA SEÑAL [%d] DE H3",sig);

    //Enviamos la señal al hijo 1, desde el hijo 2
    printf("\nSoy el HIJO 2, le voy a enviar la señal al H1 [%d]\n-------\n",pids[0]);
    kill(pids[0],SIGUSR1);    

}
void handler_h1(int sig){
    printf("SOY EL HIJO 1, RECIBI LA SEÑAL [%d] DE H2\n",sig);
    
}


int main(int argc, char * argv[]){

    signal(SIGUSR1,handler_padre);

    pids[0]=fork();

    if(pids[0]==0){//hijo 1

        signal(SIGUSR1,handler_h1);
        sleep(2);
        printf("\nSoy el H1, le voy a enviar la señal a mi padre [%d]\n-------\n",getppid());
        kill(getppid(),SIGUSR1);
        pause();
        printf("HIJO 1 FINALIZADO\n");

    }else{
        pids[1]=fork();
        if(pids[1]==0){//hijo2
            signal(SIGUSR1,handler_h2);
            pause();
            printf("HIJO 2 FINALIZADO\n");
        }else{
            pids[2]=fork();
            if(pids[2]==0){ //hijo 3
                signal(SIGUSR1,handler_h3);
                pause();
                printf("HIJO 3 FINALIZADO\n");
            }else{//padre
                for (int  i = 0; i < sizeof(pids); i++)
                {
                    wait(NULL);
                }
                printf("PADRE FINALIZADO\n");
            }
        }
    }
    



}