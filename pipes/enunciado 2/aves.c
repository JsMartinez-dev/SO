#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int error(char *msg){
    perror(msg);
    return 1;
}


void encontrarNidos(int **matriz,int inicio, int fin, int filas, int *fd,int *fd2){
    int buff1;
    int buff2;
    
    for (int i = 0; i < filas; i++)
    {
        for (int j = inicio; j < fin; j++)
        {

            if(matriz[i][j]==2){
                buff1=i;  
                buff2=j;  
                write(fd[1],&buff1,sizeof(int));
                usleep(50000);
                write(fd2[1],&buff2,sizeof(int));
                usleep(50000);
           }
        }
        
    }
    
}

int main(){

    int fd12[2];
    int fd12_2[2];
    int fd13[2];
    int fd13_2[2];
    pipe(fd12);
    pipe(fd12_2);
    pipe(fd13);
    pipe(fd13_2);
    char *archivo  ="archivo.txt";
    int filas,columnas,i,j,k;

    FILE *fp = fopen(archivo,"r");
    if(fp==NULL){
        error("Error al abrir archivo");
    }

    fscanf(fp,"%d %d",&filas,&columnas);
    int **matriz = (int **) malloc(filas * sizeof(int*));

    if(matriz==NULL){
        error("Error al reservar memoria");
    }

    printf("NUMERO DE FILAS: %d\n",filas);
    printf("NUMERO DE COLUMNAS: %d\n",columnas);

    for ( i = 0; i < filas; i++)
    {
        matriz[i] = (int*) malloc(columnas * sizeof(int));
    }
    
    for ( i = 0; i < filas; i++){
       for ( j = 0; j < columnas; j++)
        {
            fscanf(fp,"%d",&matriz[i][j]);
        }
    }
    
    for ( k = 0; k < 2; k++)
    {
        if(!fork())break;
    }


        switch (k)
        {
        case 0 :
            close(fd12[0]);
            close(fd12_2[0]);
            close(fd13[0]);
            close(fd13[1]);
            close(fd13_2[0]);
            close(fd13_2[1]);
            encontrarNidos(matriz,0,columnas/2,filas,fd12,fd12_2);
            close(fd12[1]);
            close(fd12_2[1]);
            break;
        case 1 :
            close(fd12[0]);
            close(fd12[1]);
            close(fd12_2[0]);
            close(fd12_2[1]);
            close(fd13[0]);
            close(fd13_2[0]);
            encontrarNidos(matriz,columnas/2,columnas,filas,fd13,fd13_2);
            close(fd13[1]);
            close(fd13_2[1]);
            break;
        default:
            close(fd12[1]);
            close(fd12_2[1]);
            close(fd13[1]);
            close(fd13_2[1]);
            int fila, col;
            ssize_t bytesLeidos_fila, bytesLeidos_col;
            
            while(1){
                bytesLeidos_fila = read(fd12[0], &fila, sizeof(int));
                bytesLeidos_col = read(fd12_2[0], &col, sizeof(int));
                
                if(bytesLeidos_fila <= 0 || bytesLeidos_col <= 0){
                    break;
                } 
                    
                
                printf("Nido encontrado en FILA[%d] - COLUMNA[%d]\n", fila+1, col+1);
            }
            
            while(1){
                bytesLeidos_fila = read(fd13[0], &fila, sizeof(int));
                bytesLeidos_col = read(fd13_2[0], &col, sizeof(int));
                
                if(bytesLeidos_fila <= 0 || bytesLeidos_col <= 0){
                    break;
                } 
                
                printf("Nido encontrado en FILA[%d] - COLUMNA[%d]\n", fila+1, col+1);
            }
            
            close(fd12[0]);
            close(fd12_2[0]);
            close(fd13[0]);
            close(fd13_2[0]);
            
            wait(NULL);
            wait(NULL);

            printf("\nPADRE TERMINADO.\n");
            break;
        }
    

    
    free(matriz);
    return 0;
}