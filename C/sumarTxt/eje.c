#include <stdio.h>   //-> Agrega las funciones de entrada y salida printf(), scanf(), fopen(), fclose(), fgets().
#include <stdlib.h> //-> Gestionar la memoria dinámica malloc() realloc() free() atoi()


int * guardarArray(FILE *fp1){

    /*Importante para saber cuantos numeros reservamos si conocemos la cantidad total */
    int cantidad;
    if((fscanf(fp1,"%d",&cantidad))!=1){
        perror("Error en la captura del primer numero");
        return NULL;
    }
    
    int *archivo1 = calloc(cantidad+1,sizeof(int));
    //Hasta aca
    if(archivo1==NULL){
        perror("ERROR AL ASIGNAR MEMORIA");
    }
    archivo1[0]=cantidad;
    int a;
    int contador=0;

    for (int i = 1; i <= cantidad; i++)
    {
        if((fscanf(fp1,"%d",&archivo1[i]))!=1){
            perror("Error al guardar dato");
            free(archivo1);
            return NULL;
        }
    }   
    return archivo1;
}

void sumaFinal(int *archi1,int *archi2,int *archi3,FILE *fp1){


    int mayor=archi1[0];
    int sumaF=0;

    if(archi2[0]>mayor) mayor=archi2[0];
    if(archi3[0]>mayor) mayor=archi3[0];
    
    printf("MAYOR: %d \n",mayor);
    fprintf(fp1,"%d \n",mayor);

    for (int i = 1; i <= mayor; i++)
    {
        sumaF=0;
        if(i<=archi1[0])sumaF+=archi1[i];
        if(i<=archi2[0])sumaF+=archi2[i];
        if(i<=archi3[0])sumaF+=archi3[i];
        fprintf(fp1,"%d \n",sumaF);
    }
    
}

int main(void){


    /*
    PASO 1 : leer cuantos datos tiene cada archivo, reservarlos y luego sumar sus posiciones
    */


    FILE *fp1 = fopen("a.txt","r");
    FILE *fp2 = fopen("b.txt","r");
    FILE *fp3 = fopen("c.txt","r");
    FILE *fp4 = fopen("d.txt","w");
    int *archivo1,*archivo2,*archivo3;

    if(fp1==NULL || fp2 ==NULL|| fp3 ==NULL || fp4==NULL){
        perror("ERROR AL ABRIR LOS ARCHIVOS");
        return 1;
    }

    archivo1 = guardarArray(fp1);
    archivo2 = guardarArray(fp2);
    archivo3 = guardarArray(fp3);

    if(archivo1==NULL || archivo2 ==NULL|| archivo3 ==NULL){
        perror("ERROR AL GUARDAR DATOS");
        fclose(fp1);
        fclose(fp2);
        fclose(fp3);
        fclose(fp4);
        return 1;
    }

    sumaFinal(archivo1,archivo2,archivo3,fp4);


    free(archivo1);
    free(archivo2);
    free(archivo3);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);
    fclose(fp4);

}