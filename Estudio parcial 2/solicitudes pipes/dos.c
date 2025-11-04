#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


int err(char *smg){
    perror(smg);
    return 1;
}

void mostrarResultados(char *smg,char **lista,int tam){
    printf("[%s]\n",smg);
    for (int i = 0; i < tam; i++)
    {
        printf("%s\n",lista[i]);
    }
    
}

int main(){


    int i,j,k,l,cantHijos=2,cantPipes=4,numSolicitudes;
    char *archivo = "input.txt";
    FILE *fp = fopen(archivo,"r");
    char *solicitud = malloc(1000* sizeof(char));
    char *buff = malloc(1000* sizeof(char));
    int pos=0;
    char c;

    if(fp==NULL){
        err("ERROR AL ABRIR EL ARCHIVO");
    }


    fscanf(fp,"%d",&numSolicitudes);
    int **pipes = (int **) malloc(cantPipes * sizeof(int*));
    char **urgentes = (char **)  malloc(1000 * sizeof(char*));
    char **criticas = (char **)  malloc(1000 * sizeof(char*));
    char **noImportantes = (char **)  malloc(1000 * sizeof(char*));
    int tamU=0,tamC=0,tamN=0;
    if(pipes==NULL){
        err("ERROR AL RESERVAR MEMORIA");
    }

    for ( i = 0; i < cantPipes; i++)
    {
        pipes[i]= (int*) malloc(2 * sizeof(int));
    }
    
    for ( i = 0; i < cantPipes; i++)
    {
        if(pipe(pipes[i])==-1){
            free(pipes);
            fclose(fp);
            err("ERROR AL CREAR PIPE");
        }
    }


    for ( i = 0; i < cantHijos; i++)
    {
        if(!fork()) break;
    }

    switch (i){

    case 0: //hijo1
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[2][0]);
        close(pipes[3][0]);
        close(pipes[3][1]);
        pos =0;
        while(read(pipes[0][0],&c,1)){
            solicitud[pos++]=c;
            if(c=='\0'){
                if(strncmp(solicitud,"REQ:",4)==0 && strstr(solicitud,";")!=NULL &&strstr(solicitud,"URGENTE")!=NULL){
                    write(pipes[2][1],solicitud,strlen(solicitud)+1);
                }else{
                    char *mensajeCompleto = (char*) malloc(1000 * sizeof(char));
                    
                    sprintf(mensajeCompleto,"BAJAPRIORIDAD%s",solicitud);
                    write(pipes[1][1],mensajeCompleto,strlen(mensajeCompleto)+1);
                    free(mensajeCompleto);
                }
                pos=0;
            }
        }
        break;

    case 1: //hijo2
        close(pipes[0][1]);
        close(pipes[0][0]);
        close(pipes[1][0]);
        close(pipes[1][1]);
        close(pipes[2][1]);
        close(pipes[3][0]);

        pos=0;
        while(read(pipes[2][0],&c,1)){
            solicitud[pos++]=c;
            if(c=='\0'){
                char *mensajeCompleto = (char*) malloc(1000 * sizeof(char));
                if(strstr(solicitud,"servidor")!=NULL ||strstr(solicitud,"caída")!=NULL||strstr(solicitud,"bloqueo")!=NULL){
                    sprintf(mensajeCompleto,"CRITICA%s",solicitud);
                }else{
                    sprintf(mensajeCompleto,"URGENTE%s",solicitud);           
                }
                write(pipes[3][1],mensajeCompleto,strlen(mensajeCompleto)+1);
                pos=0;
            }
        }
        
        break;

    default: //padre
        
        close(pipes[0][0]);
        close(pipes[1][1]);
        close(pipes[2][0]);
        close(pipes[2][1]);
        close(pipes[3][1]);
        //mandamos las solicitudes al hijo 1
        while(fgets(solicitud,1000,fp)){
            solicitud[strcspn(solicitud,"\n")]='\0';
            write(pipes[0][1],solicitud,strlen(solicitud)+1);
        }
        
        close(pipes[0][1]);

        //leemos las solicitudes del hijo1
        pos=0;
        while(read(pipes[1][0],&c,1)){
            buff[pos++]=c;
            if(c=='\0'){
                if(strncmp(buff,"BAJAPRIORIDAD",13)==0){
                    char *continuacion = buff+13;
                    noImportantes[tamN]= malloc(strlen(continuacion)+1);
                    strcpy(noImportantes[tamN],continuacion);
                    tamN++;
                }
                pos=0;
            }
        }
        close(pipes[1][0]);

        //leemos del hijo 2
        pos=0;
        while(read(pipes[3][0],&c,1)){
            buff[pos++]=c;
            if(c=='\0'){
                char *mensajito = (char*) malloc(1000 * sizeof(char));
                if(strncmp(buff,"CRITICA",7)==0){
                    char *continuacion = buff+7;
                    criticas[tamC]= malloc(strlen(continuacion)+1);
                    strcpy(criticas[tamC],continuacion);
                    tamC++;;
                }else{
                    char *continuacion = buff+7;
                    urgentes[tamU]= malloc(strlen(continuacion)+1);
                    strcpy(urgentes[tamU],continuacion);
                    tamU++;
                }
                pos=0;
            }
        }
        close(pipes[3][0]);
        mostrarResultados("CRITICA",criticas,tamC);
        mostrarResultados("URGENTES",urgentes,tamU);
        mostrarResultados("BAJA PRIORIDAD",noImportantes,tamN);


        for ( j = 0; j < cantHijos; j++)
        {
            wait(NULL);
        }
        printf("\nFIN PROCESO");
        break;

    }
    free(pipes);
    fclose(fp);
    return 0;
}