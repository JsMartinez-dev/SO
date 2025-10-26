#include <stdio.h>  
#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h>


int contarlineas(char * archivo){
    FILE *fp = fopen(archivo,"r");

    if(fp==NULL){
        perror("ERROR AL ABRIR EL ARCHIVO");
        return 1;
    }

    int contador=0;
    int c=0;
    int asegurador=0;

    while((c = fgetc(fp)) != EOF){
        if(c == '\n'){
            contador++;
        }
        asegurador=c;
    }


    if(asegurador!='\n' && contador>0){
        contador++; //Asegura que se leean todas las lineas
    }

    fclose(fp);
    return contador;
}

void sumaParcial(char *archivo,int inicio,int final, char *archivoSalida){

    FILE *fp = fopen(archivo,"r");
    if(fp==NULL){
        perror("ERROR AL ABRIR ARCHIVO");
    }
    int aux=0;
    double sumador=0;
    int n_lineas=0;
    int linea_actual=0;

  
    while((fscanf(fp,"%d",&aux)==1)){


        if(linea_actual>=inicio && linea_actual <final){
            sumador+=aux;
        }
        linea_actual++;

        if(linea_actual>final){
            break;
        }
    }
    

    FILE *fp2 = fopen(archivoSalida,"a");
    fprintf(fp2,"%lf",sumador);
    fprintf(fp2,"\n");
    fclose(fp);   
    fclose(fp2);
}

double sumaFinal(char *archivo1){

    FILE *fp = fopen(archivo1,"r");
 
    double suma1=0;
    double suma2=0;
    double sumaF;
    while ((fscanf(fp,"%lf",&suma1))==1)
    {
        sumaF+=suma1;
        printf("\n SUMA PARCIAL: %.1lf",sumaF);

    }
    fclose(fp);
    return sumaF;
}



int main(){

    pid_t pid = getpid();
    const char *archivo = "input.txt";
    const char *archivo2 = "input2.txt";
    const char *archivo3 = "input3.txt";
    int i,j,nHijos,c,porcion_linea,inicio,final,delta;
    double sumaf;
    int n_lineas = contarlineas(archivo);

    FILE *fp = fopen(archivo3,"r");
    
        if((fscanf(fp,"%d",&c))==1){

            for(i = 0;i<c;i++){
                if(!fork()){
                    break;
                }
            }
            

            if(pid==getpid()){ //Entra al padre
                for (j = 0; j < c; j++) //Espera a los 2 hijos
                {
                    wait(NULL);            
                }     

                sumaf = sumaFinal(archivo2);
                printf("\nLa suma final es:%.1lf",sumaf);

            }else{ //Entra en los hijos

                if(i<c){
                    delta = n_lineas/c; // 101/2 -> 50
                    delta++;

                    inicio = i*delta;  //i =0,50
                    final = inicio+delta; //f=50,101
                    printf("Numero Hijos: %d\n",c);
                    printf("Inicio: %d",inicio);
                    printf("-Fin: %d\n",final);
                    sumaParcial(archivo,inicio,final,archivo2);
                }

                }

        }else{
            perror("ERROR AL TOMAR NUMERO DE HIJOS \n");
        }

    return 0;
}
