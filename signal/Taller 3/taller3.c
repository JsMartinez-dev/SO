#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t *pids;
pid_t pidh,pidhh,pidhhh;

void mostrar(char c){
    printf("%c-",c);
    fflush(stdout);
}
void handler_padre(int sig){
    mostrar('a');
}
void handler_b(int sig){
    mostrar('b');
    kill(pidh,SIGUSR1); 
}
void handler_b_2(int sig){
    mostrar('b');
    kill(getppid(),SIGUSR1);
}
void handler_c(int sig){
    mostrar('c');
    kill(pidh,SIGUSR1);     
}
void handler_c_2(int sig){
    mostrar('c');
    kill(pids[0],SIGUSR1); 
}
void handler_d(int sig){
    mostrar('d');
    kill(pidh,SIGUSR1);
}

void handler_d_2(int sig){
    mostrar('d');
    kill(pids[1],SIGUSR1);    
}
void handler_e(int sig){
   mostrar('e');
   kill(getppid(),SIGUSR2); 
}
void handler_f(int sig){
    mostrar('f');
    kill(pidhh,SIGUSR1);    
}
void handler_f_2(int sig){
    mostrar('f');
    kill(getppid(),SIGUSR2);
}
void handler_g(int sig){
    mostrar('g');
    kill(getppid(),SIGUSR2);
    
}
void handler_h(int sig){
    mostrar('h');
    kill(pidhhh,SIGUSR1);    
}
void handler_h_2(int sig){
    mostrar('h');
    kill(getppid(),SIGUSR2);   
}
void handler_i(int sig){
     mostrar('i');
    kill(getppid(),SIGUSR2);     
}
int main(){

    signal(SIGUSR1,handler_padre);
    int cantidad =3,i,j,k;
    pids = (pid_t *) malloc(cantidad * sizeof(pid_t));

    if(pids ==NULL){
        perror("Error al reservar memoria");
        return 1;
    }

    for ( i = 0; i < cantidad; i++)
    {
        if((pids[i]=fork())==0)break;
    }
    
    if(i<cantidad){ //hijos del pade
        switch (i)
        {
        case 0: //b
            signal(SIGUSR1,handler_b);
            signal(SIGUSR2,handler_b_2);
            pidh=fork();
            if (pidh==0) //e
            {
                signal(SIGUSR1,handler_e);
                pause();
                exit(0);
            }else{
                for ( j = 0; j < 2; j++)
                {
                    pause();
                }
                exit(0);            
            }
            break;
        case 1://c
            signal(SIGUSR1,handler_c);    
            signal(SIGUSR2,handler_c_2);
            pidh=fork();
            if(pidh==0){ //f
                signal(SIGUSR1,handler_f);    
                signal(SIGUSR2,handler_f_2);    
                pidhh=fork();
                if(pidhh==0){ //h
                    signal(SIGUSR1,handler_h);    
                    signal(SIGUSR2,handler_h_2);    
                    pidhhh=fork();
                    if(pidhhh==0){//i
                        signal(SIGUSR1,handler_i);    
                        pause();
                        exit(0);
                    }else{
                         for ( j = 0; j < 2; j++){
                        pause();
                    }
                    exit(0);  
                    }
                }else{
                  for ( j = 0; j < 2; j++){
                        pause();
                    }
                    exit(0);     
                }
            }else{
                for ( j = 0; j < 2; j++) {
                    pause();
                }
                exit(0);  
            }

            break;
        case 2://hijo 3
            signal(SIGUSR1,handler_d);    
            signal(SIGUSR2,handler_d_2);  
            pidh = fork();
            if(pidh==0){ //g
                signal(SIGUSR1,handler_g);
                pause();
                exit(0);
            }  else{
                for ( j = 0; j < 2; j++){
                    pause();
                }
                exit(0);   
            }
            break;
        default:
            perror("ERROR DE INDICA");
            break;
        }
    }else{ //padre
        printf("\n-___-COMENZANDO -___-\n");  
        mostrar('a');
        sleep(2);
        kill(pids[2],SIGUSR1);
        for (k = 0; k < cantidad; k++)
        {
            wait(NULL);
        }
        printf("\n-___- PADRE FINALIZADO -___-\n");  

    }




    return 0;
}

