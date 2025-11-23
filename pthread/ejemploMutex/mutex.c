#include <stdio.h>    
#include <unistd.h>    
#include <stdlib.h>     
#include <pthread.h>
#include <string.h>

pthread_mutex_t mi_mutex;
int cont=0;


void *contador(void *arg){

    int *numero = (int *)arg;

    for (int  i = 0; i < 100; i++)
    {
        pthread_mutex_lock(&mi_mutex);
        cont+=*numero;
        pthread_mutex_unlock(&mi_mutex);
    }
    

    
}

int main(){


    int nHilosMax,i,j,k;
    printf("Digita la cantidad de hilos: ");
    scanf("%d",&nHilosMax);
    int *dato;
    dato = malloc(sizeof(int));
    *dato=1;
    pthread_mutex_init(&mi_mutex,NULL);

    pthread_t *pids = (pthread_t*) malloc(nHilosMax* sizeof(pthread_t));

    for ( i = 0; i < nHilosMax; i++)
    {
        
        pthread_create(&pids[i],NULL,contador,(void* )dato);

    }
    
    for ( i = 0; i <nHilosMax; i++)
    {
        pthread_join(pids[i],NULL);
    }

    printf("Suma total: %d\n",cont);
    printf("PROCESO FINALIZADO\n");
    pthread_mutex_destroy(&mi_mutex);
    



    return EXIT_SUCCESS;
}