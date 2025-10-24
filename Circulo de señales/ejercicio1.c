#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>


pid_t pidHijos[2];
int vueltas_totales;
int vueltas_restantes;

void handler_hijo1(int sig){
    printf("A H1 LE LLEGO LA SEÑAL [%d] DE H2\n",sig);
    vueltas_restantes--;

    if(vueltas_restantes>0){

      printf("\n\n----VUELTA NUMERO: %d ----\n\n",vueltas_totales-vueltas_restantes+1);
      printf("SOY HIJO 1, MANDO SEÑAL AL PADRE [%d]\n",getppid());
      sleep(1);
      kill(getppid(),SIGUSR1); 

    }
}

void handler_hijo2(int sig){
    printf("A H2 LE LLEGO LA SEÑAL [%d] DE PADRE\n",sig);
    sleep(1);
    kill(pidHijos[0],SIGUSR1);
}
void handler_padre(int sig){
    printf("A PADRE LE LLEGO LA SEÑAL [%d] DE H1\n",sig);
    kill(pidHijos[1],SIGUSR1);
}

int main(int argc, char *argv[]){
    
    if(argc < 2){
        printf("Uso: %s <número_de_vueltas>\n", argv[0]);
        return 1;
    }
    
    vueltas_totales = atoi(argv[1]);
    vueltas_restantes = vueltas_totales;

    if(vueltas_totales<=0){
        perror("Numero de vueltas invalidas");
        return 1;
    }

    signal(SIGUSR1,handler_padre);

    pidHijos[0]=fork();

    if(pidHijos[0]==0){ //hijo 1
        signal(SIGUSR1,handler_hijo1);
        sleep(1);
        printf("SOY HIJO 1, MANDO SEÑAL AL PADRE [%d]\n",getppid());

        kill(getppid(),SIGUSR1);
        while(vueltas_restantes>0){
            pause(); 
        }
        printf("HIJO 1 FINALIZADO...\n");
        
    }else{
        pidHijos[1]=fork();
        if(pidHijos[1]==0){ //hijo 2 -> aqui [0] es el PID del H1
            signal(SIGUSR1,handler_hijo2);
            printf("SOY HIJO 2, MANDO SEÑAL AL HIJO 1 [%d]\n",pidHijos[0]);           
            for (int i = 0; i < vueltas_totales; i++){
                pause();
            }

            printf("HIJO 2 FINALIZADO...\n");
        }else{
            printf("SOY PADRE, MANDO SEÑAL A HIJO 2 [%d]\n",pidHijos[1]);
            sleep(1); //Espera a que todo se configure en el programa
            wait(NULL);
            wait(NULL);
            printf("PADRE FINALIZADO...\n");
        }
    }

    return 0;
}