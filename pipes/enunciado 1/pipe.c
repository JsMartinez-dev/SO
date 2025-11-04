#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int error(const char *err){ 
    perror(err);
    return 1;
}

int main(){

    int fd[2];
    pid_t pid;
    char buffer[100];
    
    if(pipe(fd)==-1){
        error("Pipe no creado");
    }
    

    pid=fork();

    if(pid==0){
        close(fd[1]);
        usleep(1000);
        read(fd[0],buffer,sizeof(buffer));
        printf("Mensaje enviado: %s",buffer);
        close(fd[0]);
    }else{
        char *mensaje = "HOLA QUE TAL\n";
        close(fd[0]);
        write(fd[1],mensaje,strlen(mensaje));
        close(fd[1]);
    }



    return 0;
}