    #include <stdio.h>
    #include <unistd.h>
    #include <string.h>
    #include <stdlib.h>
    #include <sys/types.h>
    #include <sys/wait.h>

    int errores(char *msg){
        perror(msg);
        return 1;
    }

    int main(){
        //Esto es un acambio en la rama feature/user
        int NHijos,i,j,k;
        char *smg = (char *)  malloc( 100 * sizeof(char));
        char *buf = (char *)  malloc( 100 * sizeof(char));
        ssize_t cantBytes;
        
        printf("Indica el numero de hijos: ");
        scanf("%d",&NHijos);
        int cantPipes = NHijos*2;

        printf("Indica el mensaje a mandar: ");
        scanf("%s",smg);

        
        int **pipes= (int **) malloc((cantPipes)* sizeof(int *));

        for ( i = 0; i < cantPipes; i++)
        {
            pipes[i] = (int *) malloc(2 * sizeof(int));
        }
        
        //Creamos las tuberias para cada hijo

        for ( i = 0; i < cantPipes; i++)
        {
            if(pipe(pipes[i])==-1){
                errores("Error al crear la pipe");
            }
        }

        //Creamos los hijos
        for ( i = 0; i < NHijos; i++)
        {
            if(!fork()){
    

            
    
    
                close(pipes[i][1]);
                cantBytes = read(pipes[i][0],buf,sizeof(buf));
                if(cantBytes>0){
                        printf("PID[%d]-MENSAJE:%s\n",getpid(),buf);
                }
                close(pipes[i][0]);
                close(pipes[i+1][0]);
                write(pipes[i+1][1],buf,strlen(buf)+1);
                usleep(1000);
                close(pipes[i+1][1]);


                //Devolver el mensaje
                close(pipes[cantPipes-i-2][1]);
                cantBytes = read(pipes[cantPipes-i-2][0],buf,sizeof(buf));
                if(cantBytes>0){
                        printf("PID[%d]-MENSAJE:%s\n",getpid(),buf);
                }
                close(pipes[cantPipes-i-1][0]);
                cantBytes = write(pipes[cantPipes-i-1][1],buf,strlen(buf)+1);
                usleep(1000);
  
                close(pipes[cantPipes-i-1][1]);
                    

                    exit(0);
            }
        }

            for(int p = 1; p < cantPipes-1; p++){
                    close(pipes[p][0]);
                    close(pipes[p][1]);
                }
            printf("PADRE PID[%d]-MENSAJE:%s\n",getpid(),smg);
            
                
            close(pipes[0][0]);
            cantBytes= write(pipes[0][1],smg,strlen(smg)+1);
            if(cantBytes<0){
                errores("SE MANDO MAL EL MENSAJE\n");
            }
            close(pipes[0][1]);
            
            
            close(pipes[cantPipes-1][1]);
            cantBytes= read(pipes[cantPipes-1][0],buf,sizeof(buf));
            if(cantBytes>0){
                printf("PADRE PID[%d]-MENSAJE :%s\n",getpid(),buf);
            }else{
                errores("SE MANDO MAL EL MENSAJE\n");
            }
            close(pipes[cantPipes-1][0]);
            
            
            
            for (int p = 0; p < NHijos; p++)
            {
                wait(NULL);
            }
            printf("\n\nPROCESO FINALIZADO\n");

            // Liberar memoria
            free(smg);
            free(buf);
            for(i = 0; i < cantPipes; i++){
                free(pipes[i]);
            }
            free(pipes);

            return 0;
}