    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/wait.h>


int errores(char *smg){
        perror(smg);
        return 1;
}

void mostrarResultados(char **lista,int tam,char *clasificacion){
    printf("[%s]\n",clasificacion);
    for (int i = 0; i<tam; i++)
    {
        printf("%s\n",lista[i]);
    }
    
}

int main(){

    char *archivo = "input.txt";
    int capacidadSolicitudes,cantPipes=4,i,j,k;
    int tamC=0,tamU=0,tamB=0;
    FILE *fp = fopen(archivo,"r");
    ssize_t cantBytes;
    ssize_t cantBytes2;
    int pos =0;

    char c;

    if(fp==NULL){
        errores("Lectura fallida del archivo");
    }

    fscanf(fp,"%d",&capacidadSolicitudes);

    char *lista = (char *) malloc(1000 * sizeof(char));
    char **lCritica = (char **) malloc(1000 * sizeof(char*));
    char **lUrgente = (char **) malloc(1000 * sizeof(char*));
    char **lBajaPrioridad = (char **) malloc(1000 * sizeof(char*));
    char *buff = (char *) malloc(1000 * sizeof(char));
    char *buff2 = (char *) malloc(1000 * sizeof(char));


    //Creamos los pipes
    int** pipes = (int **) malloc( cantPipes * sizeof(int *));
    for ( i = 0; i < cantPipes; i++)
    {
        pipes[i] = (int *) malloc(2 * sizeof(int));
    }
    for ( i = 0; i < cantPipes; i++)
    {
        if(pipe(pipes[i])==-1){
            errores("Error en crear los pipes");
            free(pipes);
            free(lista);
        }
    }
    
    for ( i = 0; i < 2; i++)
    {
        if(!fork())break;
    }
    

    switch (i)
    {
    case 0: //hijo1
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[2][0]);
        close(pipes[3][0]);
        close(pipes[3][1]);

        pos=0;
        while(read(pipes[0][0],&c,1)){
            buff[pos++]=c;

            if(c=='\0'){
                if(strncmp(buff,"REQ:",4)==0 && strstr(buff,";")!=NULL &&strstr(buff,"URGENTE")!=NULL){
                    write(pipes[2][1],buff,strlen(buff)+1);
                }else{
                    write(pipes[1][1],buff,strlen(buff)+1);
                }
                pos=0;
            }

        }
        close(pipes[2][1]);
        close(pipes[0][0]);
        close(pipes[1][1]);
        break;
    case 1: //Hijo2

        close(pipes[0][0]);
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[1][1]);
        close(pipes[2][1]);
        close(pipes[3][0]);

         pos=0;
         while(read(pipes[2][0],&c,1)){
            buff[pos++]=c;

            if(c=='\0'){
                char *cadenaCompleta = (char*) malloc(1000 * sizeof(char));
                if(strstr(buff,"ervidor")!=NULL ||strstr(buff,"bloqueo")!=NULL ||strstr(buff,"caída")!=NULL){
                    sprintf(cadenaCompleta,"CRITICA%s",buff);
                }else{
                    sprintf(cadenaCompleta,"URGENTE%s",buff);
                }
                write(pipes[3][1],cadenaCompleta,strlen(cadenaCompleta)+1);
                free(cadenaCompleta);
                pos=0;
            }
         }

        close(pipes[2][0]);
        close(pipes[3][1]);
        break;

    default: //padre

        close(pipes[0][0]);
        close(pipes[1][1]);
        close(pipes[2][0]);
        close(pipes[2][1]);
        close(pipes[3][1]);

        while(fgets(lista,1000,fp)){ //Mandamos todas las solicitudes al hijo 1 primero
            lista[strcspn(lista, "\n")] = '\0';
            write(pipes[0][1],lista,strlen(lista)+1);
        }
        close(pipes[0][1]);

        pos=0;
        while(read(pipes[1][0],&c,1)){ //Leemos caracter por caracter las cadenas del hijo 1
            buff[pos++]=c;
            if(c=='\0'){
                lBajaPrioridad[tamB] = malloc(strlen(buff)+1);
                strcpy(lBajaPrioridad[tamB],buff);
                tamB++;
                pos=0;
            }
        }

        pos=0;
        while(read(pipes[3][0],&c,1)){
            buff2[pos++]=c;
            if(c=='\0'){
                if(strncmp(buff2,"CRITICA",7)==0){
                    char *continuacion = buff2+7;
                    lCritica[tamC] = malloc(strlen(continuacion)+1);
                    strcpy(lCritica[tamC],continuacion);
                    tamC++;
                }else{ //Entonces se concluye que son peticione urgentes
                    char *continuacion = buff2+7;
                    lUrgente[tamU] = malloc(strlen(continuacion)+1);
                    strcpy(lUrgente[tamU],continuacion);
                    tamU++;
                }
                pos=0;
            }
        }
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[3][0]);
        
        mostrarResultados(lCritica,tamC,"CRITICA");
        mostrarResultados(lUrgente,tamU,"URGENTE");
        mostrarResultados(lBajaPrioridad,tamB,"BAJA PRIORIDAD");

        for ( k = 0; k < 2; k++)
        {
            wait(NULL);
        }
        printf("\nFin del proceso\n");

        break;
    }

    free(pipes);
    free(lista);
    free(buff);
    free(buff2);
    return 0;
}