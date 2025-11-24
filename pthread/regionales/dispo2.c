#define _POSIX_C_SOURCE 200809L 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <stddef.h>
#include <pthread.h>
#include <ctype.h>
typedef struct inter{

    int ini;
    int fin;
}Inter;


int cantAchivos=0;

int cant_us=0;
int cant_eu=0;
int cant_sa=0;
int canTiempo=0;
int num_hilos=0;
int delta=0;
char **listArchivos;
int cant_informes=0;
int i,j,k;

pthread_mutex_t miMutex = PTHREAD_MUTEX_INITIALIZER;

void err(char *smg){
    perror(smg);
    abort();
}

void print_resul(){
    printf("\n--__--RESULTADOS--__--\n");
    printf("us-east:%d\n ",cant_us);
    printf("eu-west:%d \n",cant_eu);
    printf("sa-east:%d \n",cant_sa);
    
}
 
void readFile( const char *file){

    FILE *fp = fopen(file,"r");

    if(!fp){
        err("File");
    }

    fscanf(fp,"%d",&cantAchivos);
    fgetc(fp); //Consume el \n
    
    listArchivos = (char**) malloc(cantAchivos*sizeof(char*));
    for ( i = 0; i < cantAchivos; i++)
    {
        listArchivos[i] = (char*) malloc(1000 * sizeof(char));
        fgets(listArchivos[i],1000,fp);
        listArchivos[i][strcspn(listArchivos[i], "\n")] = '\0';
    }
    

    
}

void * funHilos(void *interval){

    Inter *inter = (Inter*) interval;

    printf("Ini: %d - Fin: %d\n",inter->ini,inter->fin);
    
    
    for ( i = inter->ini; i < inter->fin; i++)
    {
        FILE *fp2 = fopen(listArchivos[i],"r");
        fscanf(fp2,"%d",&cant_informes);
        fgetc(fp2);
        printf("Cantdiad de informes: %d\n",cant_informes);

        if(!fp2){
            err("Archivo");
        }

        char *smg = (char *) malloc(1000 * sizeof(char));
        while(fgets(smg,1000,fp2)){

            int * contador =NULL;
            
            if(strstr(smg,"us-east")){//puntero donde empieza la palabra
                contador = &cant_us;
            }else if(strstr(smg,"eu-west")){
                contador = &cant_eu;
            }else if (strstr(smg,"sa-east")){
                contador = &cant_sa;
            }
            
            if(contador!=NULL){
                char *pos = strstr(smg,"FAIL;");
                if(pos!=NULL){
                    pos +=5;
                    int numero =atoi(pos); 
                    
                    pthread_mutex_lock(&miMutex);
                    *contador+= numero;
                    pthread_mutex_unlock(&miMutex);
                }
            }
        }
        fclose(fp2);
    }
  
    pthread_exit(0);
}

int main(int argc, char**argv){


    readFile(argv[1]);


   
    num_hilos = atoi(argv[2]);
    pthread_t* indx = (pthread_t*) malloc(num_hilos *sizeof(pthread_t));

    delta = cantAchivos/num_hilos;

    for ( i = 0; i < num_hilos; i++)
    {
        Inter* inter = (Inter*) malloc(sizeof(Inter));
        inter->ini = delta * i;
        inter->fin = inter->ini+delta;

        pthread_create(&indx[i],NULL,funHilos,(void *)inter);
    }

    
    for ( i = 0; i < num_hilos; i++){
        pthread_join(indx[i],NULL);
   }


   print_resul();

    free(indx);
    free(listArchivos);
    pthread_mutex_destroy(&miMutex);
    return EXIT_SUCCESS;
}