#define _POSIX_C_SOURCE 200809L 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>



int error(char *Smg){
    perror(Smg);
    return 1;
}


float maxGLobal;
float **matriz;
int filas,columnas,i,j,k;
pthread_barrier_t mi_barrera;



void *funHilo(void *arg){

int *array = (int * )arg;
float maxLocal=0;
float sumita=0;

float mayor=0;
    for (int i = array[0]; i < array[1]; i++){
        for (int j = 0; j <columnas ; j++)
        {
            sumita += matriz[i][j];
        }
        printf("La suma total es: %f \n",sumita);
        for (int j = 0; j <columnas ; j++)
        {
            float global = matriz[i][j]/sumita;
            matriz[i][j] = global;
        }
        sumita=0;
    }

    printf("matriz:\n");
    for (int i = array[0]; i < array[1]; i++){
        for (int j = 0; j <columnas ; j++)
        {
            
            printf("[%f]",matriz[i][j]);
        }
        printf("\n");
    }

    for (int i = array[0]; i < array[1]; i++){
        for (int j = 0; j <columnas ; j++)
        {
            if(mayor<matriz[i][j]){
                mayor=matriz[i][j];
            }
        }
    }

    if(maxGLobal<mayor){
        maxGLobal=mayor;
    }
    pthread_barrier_wait(&mi_barrera);

    printf("GLOBAL MAXIMO: %f\n",maxGLobal);
    for (int i = array[0]; i < array[1]; i++){
        for (int j = 0; j <columnas ; j++)
        {
            float global = matriz[i][j]/maxGLobal;
            matriz[i][j] = global;
        }
    }   


    pthread_exit(0);

}

int main(){

    char *archivo  ="archivo.txt";
    int numHilos =3;
    int delta, inicio,final;
    int *index;
    pthread_barrier_init(&mi_barrera, NULL, numHilos);

    FILE *fp = fopen(archivo,"r");
    if(fp==NULL){
        error("Error al abrir archivo");
    }

    fscanf(fp,"%d %d",&filas,&columnas);
    matriz = (float **) malloc(filas * sizeof(float*));

    if(matriz==NULL){
        error("Error al reservar memoria");
    }

    printf("NUMERO DE FILAS: %d\n",filas);
    printf("NUMERO DE COLUMNAS: %d\n",columnas);

    for ( i = 0; i < filas; i++)
    {
        matriz[i] = (float*) malloc(columnas * sizeof(float));
    }
    
    for ( i = 0; i < filas; i++){
       for ( j = 0; j < columnas; j++)
        {
            fscanf(fp,"%f",&matriz[i][j]);
        }
    }

    pthread_t *pids = (pthread_t*) malloc(filas* sizeof(pthread_t));




     for ( i = 0; i < numHilos; i++)
    {     
        delta = filas/numHilos;
        inicio= i*delta;
        final = inicio+delta;
        index = (int *) malloc(2 * sizeof(int));
        index[0] =inicio;
        index[1] =final;
        pthread_create(&pids[i],NULL,funHilo,(void* )index);

    }


    for ( i = 0; i <filas; i++){
        pthread_join(pids[i],NULL);
    }

    printf("matriz final:\n");

    for (int i = 0; i < filas; i++){
        for (int j = 0; j <columnas ; j++)
        {
            
            printf("[%f]",matriz[i][j]);
        }
        printf("\n");
    }

    printf("El mayor de toda la matriz es: %f\n",maxGLobal);
    printf("FIN DEL PROCESO\n");

    pthread_barrier_destroy(&mi_barrera);
    free(matriz);
    return EXIT_SUCCESS;
}