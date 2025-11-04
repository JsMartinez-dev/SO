#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

const char *keywords[] = { "int", "float", "return", "if", "else", "for", "while", "printf" };

int isKeyword(const char *str) {
    int numKeywords = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < numKeywords; i++) {
        if (strstr(str,keywords[i])!=NULL) {
            return 1;
        }
    }
    return 0;
}

int err(char * smg){
    perror(smg);
    return 1;
}

int main(){

    int cLineas=0;
    char *archivo="input.txt";
    FILE *fp = fopen(archivo,"r");
    int i,j,k,l,cantPipes=6;
    char *linea = (char *) malloc(1000 * sizeof(char));
    char c;
    int pos=0;
    if(fp==NULL){
        err("ERROR AL ABRIR EL ARCHIVO");
    }

    int **pipes = (int **) malloc(cantPipes * sizeof(int *));
    for ( i = 0; i < cantPipes; i++)
    {
        pipes[i] = (int *) malloc(2 * sizeof(int));
    }
    
    for ( i = 0; i < cantPipes; i++)
    {
        if(pipe(pipes[i])==-1){
            err("NO CREADA PIPE");
        }
    }

    for ( i = 0; i < 3; i++)
    {
        if(!fork())break;
    }
    
    switch (i)
    {
    case 0: //h1
        
        close(pipes[0][1]);
        close(pipes[1][0]);
        close(pipes[2][0]);
        close(pipes[2][1]);
        close(pipes[3][1]);
        close(pipes[3][0]);
        close(pipes[4][0]);
        close(pipes[4][1]);
        close(pipes[5][1]);
        close(pipes[5][0]);

        pos=0;
        while(read(pipes[0][0],&c,1)){

            linea[pos++]=c;
            if(c=='\0'){
                if(strlen(linea)>0){
                    cLineas++;
                }
                pos=0;
            }
        }
        close(pipes[0][0]);
        write(pipes[1][1],&cLineas,sizeof(cLineas));
        close(pipes[1][1]);
        break;

    case 1: //h2
        close(pipes[0][1]);
        close(pipes[0][0]);
        close(pipes[1][0]);
        close(pipes[1][1]);
        close(pipes[2][1]);
        close(pipes[3][0]);
        close(pipes[4][0]);
        close(pipes[4][1]);
        close(pipes[5][1]);
        close(pipes[5][0]);

        pos=0;
        while(read(pipes[2][0],&c,1)){

            linea[pos++]=c;
            if(c=='\0'){
                if(isKeyword(linea)>0){
                    cLineas++;
                }

                pos=0;
            }
        }
        close(pipes[2][0]);
        write(pipes[3][1],&cLineas,sizeof(cLineas));
        close(pipes[3][1]);
        break;
    case 2: //hijo 3
        close(pipes[0][1]);
        close(pipes[0][0]);
        close(pipes[1][0]);
        close(pipes[1][1]);
        close(pipes[2][1]);
        close(pipes[2][0]);
        close(pipes[3][0]);
        close(pipes[3][1]);
        close(pipes[4][1]);
        close(pipes[5][0]);

        pos=0;
        while(read(pipes[4][0],&c,1)){

            linea[pos++]=c;
            if(c=='\0'){
                if(strstr(linea,"//")!=NULL){
                    cLineas++;
                }

                pos=0;
            }
        }
        close(pipes[4][0]);
        write(pipes[5][1],&cLineas,sizeof(cLineas));
        close(pipes[5][1]);
        
        break;
    default:
        int cant1,cant2,cant3;
        close(pipes[0][0]);
        close(pipes[1][1]);
        close(pipes[2][0]);
        close(pipes[3][1]);
        close(pipes[4][0]);
        close(pipes[5][1]);

        while(fgets(linea,1000,fp)){  //Mandamos las lineas del codigo a los hijos
            linea[strcspn(linea, "\n")] = '\0';
            write(pipes[0][1],linea,strlen(linea)+1);
            write(pipes[2][1],linea,strlen(linea)+1);
            write(pipes[4][1],linea,strlen(linea)+1);
        }
           
            close(pipes[0][1]);
            close(pipes[2][1]);
            close(pipes[4][1]);
        
        printf("\n--__-Resultados--___--\n");
        while(read(pipes[1][0],&cant1,sizeof(cant1))){ // leemos del hijo 1
            printf("\nCantidad de lineas de codigo: %d\n",cant1);
        }
        while(read(pipes[3][0],&cant2,sizeof(cant2))){ //leemos del hijo 2
            printf("Cantidad de palabras clave: %d\n",cant2);
        }
        while(read(pipes[5][0],&cant3,sizeof(cant3))){ //leemos del hijo 3
            printf("Cantidad de comentarios: %d\n",cant3);
        }
        close(pipes[0][1]);
        close(pipes[2][1]);
        close(pipes[4][1]);
        close(pipes[1][0]);
        close(pipes[3][0]);
        close(pipes[5][0]);

        for ( j = 0; j < 3; j++)
        {
            wait(NULL);
        }
        
        break;
    }

}