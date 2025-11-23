#define _POSIX_C_SOURCE 200809L 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <stddef.h>
#include <pthread.h>

typedef struct Inter{
    int ini;
    int fin;
}Inter;



float** matA = NULL;
float** matB = NULL;



int rows =0;
int col =0;
int delta=0;
int num_hilos=0;
int num_dias =0;
int hilos_terminados=0;
int continuar=0;
int i,j,k,l;

pthread_barrier_t barrier;
pthread_cond_t con_seguir = PTHREAD_COND_INITIALIZER;
pthread_cond_t con_fin = PTHREAD_COND_INITIALIZER;
pthread_mutex_t miMutex = PTHREAD_MUTEX_INITIALIZER;


void err(char *smg){
    perror(smg);
    abort();
}

void readFile( const char *file){

    FILE *fp = fopen(file,"r");

    if(!fp){
        err("File");
    }

    fscanf(fp,"%d",&rows);
    fscanf(fp,"%d",&col);

    matA = (float **) malloc(rows * sizeof(float*));
    matB = (float **) malloc(rows * sizeof(float*));

    if(!matA || !matB){
        err("Matrices");
    }

    for ( i = 0; i < rows; i++)
    {
        matA[i] = (float *) malloc(col * sizeof(float));
        matB[i] = (float *) malloc(col * sizeof(float));
        if(!matA[i] || !matB[i]){
            err("Columnas");
        }
    }
    

    for ( i = 0; i < rows; i++){
        for ( j = 0; j < col; j++){
            fscanf(fp,"%f",&matA[i][j]);
            matB[i][j]=matA[i][j];

        }
    }
    
}

void mostrarMatriz(float **M,int rows, int cols, int dia){
    printf("--DIA: %d--\n",dia);
    for ( i = 0; i < rows; i++)
    {
        for ( j = 0;j < cols; j++)
        {
            printf("[%f]",M[i][j]);
        }
        printf("\n");
    } 
    printf("-----------------");
}

float core(float **M,int i, int j){
    return (M[i-1][j] +M[i+1][j]+M[i][j-1]+M[i][j+1])/4;
}

void * matJacobi(void *interval){

    Inter* inter = (Inter*)interval;
    //printf("Inicio: %d - Fin %d \n",inter->ini,inter->fin);
    
    for (int dia = 0; dia < num_dias; dia++){
        pthread_mutex_lock(&miMutex);
        continuar=0;
        pthread_mutex_unlock(&miMutex);
        for ( i = inter->ini; i < inter->fin; i++)
        {
            for ( j = 1;  j< col-1; j++)
            {
                if(dia%2==0){
                    matB[i][j] = core(matA,i,j); 
                }else{
                    matA[i][j] = core(matB,i,j); 
                }
            }
        }
        pthread_mutex_lock(&miMutex);
        hilos_terminados++;

        if(hilos_terminados==num_hilos){
            pthread_cond_signal(&con_seguir); //Aqui llega el ultimo hilo, y le manda la señal al pae
        }

        while(continuar==0){
            pthread_cond_wait(&con_fin,&miMutex);
        }
        pthread_mutex_unlock(&miMutex);
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(0);

} 



int main(int argc, char**argv){


    readFile(argv[1]);
    // mostrarMatriz(matA,rows,col);


    num_hilos = atoi(argv[2]);
    num_dias = atoi(argv[3]);


   pthread_t* indx = (pthread_t *) malloc(num_hilos * sizeof(pthread_t));
    pthread_barrier_init(&barrier,NULL,num_hilos);

   /*
    delta = filas/num_hilos;
    inicio = delta * i
    fin = delta +inicio;
   */

   int n_rows = rows-2;
   delta = n_rows/num_hilos;


   for ( i = 0; i < num_hilos; i++){
    
        Inter* inter = (Inter*) malloc(sizeof(Inter));

        inter->ini = delta * i+1;
        inter->fin = (i == num_hilos-1) ? rows-1 : inter->ini+delta;
        pthread_create(&indx[i],NULL,matJacobi,(void *)inter);

   }



   for (int  dia = 0; dia < num_dias; dia++){

        pthread_mutex_lock(&miMutex);
        while (hilos_terminados<num_hilos){
            pthread_cond_wait(&con_seguir,&miMutex);
        }
        float **matAux = (dia%2==0) ? matB : matA;
        mostrarMatriz(matAux,rows,col,dia+1);

        hilos_terminados=0;
        continuar=1;
        pthread_cond_broadcast(&con_fin);

        
        pthread_mutex_unlock(&miMutex);

   }
   

   for ( i = 0; i < num_hilos; i++){
        pthread_join(indx[i],NULL);
   }


   pthread_barrier_destroy(&barrier);
   pthread_mutex_destroy(&miMutex);
   pthread_cond_destroy(&con_seguir);
   pthread_cond_destroy(&con_fin);

   return EXIT_SUCCESS;


}