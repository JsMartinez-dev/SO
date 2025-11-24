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
int i,j,k;
void err(char *smg){
    perror(smg);
    abort();
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

    //printf("Ini: %d - Fin: %d\n",inter->ini,inter->fin);
    
    
    for ( i = inter->ini; i < inter->fin; i++)
    {
        FILE *fp2 = fopen(listArchivos[i],"r");

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
                    *contador+= numero;
                }
            }

         
        }

    }
    

    
    pthread_exit(0);
}

int main(int argc, char**argv){


    readFile(argv[1]);

    for ( i = 0; i < cantAchivos; i++)
    {
       printf("Nombre archivo: %s\n",listArchivos[i]);
    }
   
    num_hilos = atoi(argv[2]);
    pthread_t* indx = (pthread_t*) malloc(num_hilos *sizeof(pthread_t));

    delta = cantAchivos/num_hilos;

    for ( i = 0; i < num_hilos; i++)
    {
        Inter* inter = (Inter*) malloc(sizeof(Inter));
        inter->ini = delta * i+1;
        inter->fin = (i == num_hilos-1) ? cantAchivos : inter->ini+delta;

        pthread_create(&indx[i],NULL,funHilos,(void *)inter);
    }

    
    for ( i = 0; i < num_hilos; i++){
        pthread_join(indx[i],NULL);
   }


return EXIT_SUCCESS;
}