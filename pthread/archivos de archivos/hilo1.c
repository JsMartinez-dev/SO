#include <stdio.h>    
#include <unistd.h>    
#include <stdlib.h>     
#include <pthread.h>
#include <string.h>


void* mostrarNumero(void *arg);

int main(){

    int i,j,k,l;
    char *archivo = "archivo.txt";

    int cantMax=3;

    pthread_t tids[cantMax];

    FILE *fp = fopen(archivo,"r");

    int cant;
    fscanf(fp,"%d",&cant);

    printf("Hay esta cantidad de archivo: %d\n",cant);
    fgetc(fp); //Consumimos el \n
    char **listaArchivos = (char **) malloc(cant * sizeof(char*));
    
    for ( i = 0; i < cant; i++)
    {
        listaArchivos[i] = (char *) malloc(1000 * sizeof(char));
        fgets(listaArchivos[i],1000,fp);
        listaArchivos[i][strcspn(listaArchivos[i], "\n")] = '\0';
    }

    for ( i = 0; i < cant; i++)
    {
        printf("Archivos: %s\n",listaArchivos[i]);
    }
    

    
    //Creamos los hilos
    for ( i = 0; i < cantMax; i++)
    {
        pthread_create(&tids[i],NULL,mostrarNumero,(void * )listaArchivos[i]);
        usleep(100);
    }

    for ( i = 0; i < cantMax; i++)
    {
        pthread_join(tids[i],NULL); //Esperamos a los hilos
    }
    
    
    printf("ID DEL HILO PRINCIPAL [%lu]",pthread_self());

    return 0;
    free(listaArchivos);
    fclose(fp);

}

void* mostrarNumero(void *arg){
    char * archivoPrueba = (char *)arg;
    int numero=0;
    FILE *fp2 = fopen(archivoPrueba,"r");
    if(fp2==NULL){
        printf("No se pudo abrir el archivo: %s\n", archivoPrueba);
        return NULL;
    }
    fscanf(fp2,"%d",&numero);

    printf("Numero guardado:[%d]-por el hilo[%lu]\n",numero,pthread_self());
    fclose(fp2);
    pthread_exit(0);
}