#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>

void rle_encode(char *s){
    int conteo=1;
    int capacidadCadena= strlen(s);
    for(int i=0;i<capacidadCadena;i++){
        if(s[i]==s[i+1]){
            conteo++;
        }else{
            printf("%c%d",s[i],conteo);
            conteo=1;
        }
    }
}

void rle_decode(char *s){
    int capacidadCadena= strlen(s);
    char cadenaTemp[capacidadCadena];
    char caracterTemporal;
    int tempoDigito;
    char auxiliar[2];

    cadenaTemp[0] = '\0'; //Para que elimine basura ahí
    
    for(int i=0;i<capacidadCadena;i++){
        if(isalpha(s[i])){
            caracterTemporal=s[i];
        }
        if(isdigit(s[i])){
            while(isdigit(s[i])){
                auxiliar[0]=s[i];
                auxiliar[1]='\0';
                strcat(cadenaTemp,auxiliar);
                i++;
            }
            tempoDigito = atoi(cadenaTemp);
            imprimirTexto(caracterTemporal,tempoDigito);
            cadenaTemp[0] = '\0'; //Para que borre lo que aguardo anteriormente 
            i--;
        }
    }

}

void imprimirTexto(char caracterT,int intT){
    for(int i=0;i<intT;i++){
        printf("%c",caracterT);
    }
}

char* leerCadenaDinamica(FILE *fp){
    
    int capacidad =10;
    char *cadena = malloc(capacidad * sizeof(char));
    int cTemp;
    char *temp;
    int espacioTemp=0;

    if(cadena ==NULL){
        printf("Error al ocupar memoria");
    }


    //Vamos leyendo caracter por caracter
    while((cTemp = fgetc(fp)) !='\n' && cTemp != EOF && cTemp != ' '){
        
        if(espacioTemp>1000000){
            printf("\n\nEl ESPACIO DE CADENA EXCEDE EL LIMITE");
            return NULL;
        }
        if(espacioTemp>=capacidad-1){
            capacidad = capacidad*2;
            

            temp  = realloc(cadena, capacidad * sizeof(char));
            if(temp ==NULL){
                printf("Error en el redimensionamiento");
                free(cadena);
                return NULL;
            }
            cadena= temp;
        }

        cadena[espacioTemp] = (char) cTemp;
        espacioTemp++;
    }
    cadena[espacioTemp] = '\0';

    return cadena;
}

int main(){
    char mode[7];
    const char *archivo  ="archi.txt";
    FILE *fp = fopen(archivo,"r");
    int c;
  
    if(fp==NULL){
        return 1;
    }
    
    fscanf(fp,"%s",mode); //Guardamos primera linea de codigo

    while((c = fgetc(fp))!=EOF && c!='\n'); // Avanza hasta la segund linea
    

    char *cadena = leerCadenaDinamica(fp);
    if (cadena == NULL) {
        fclose(fp);
        return 1;
    }
    
    if(strcmp(mode,"ENCODE")==0){
        rle_encode(cadena);
    }else if(strcmp(mode,"DECODE")==0){
        rle_decode(cadena);
    }else{
        free(cadena);
        return 1;
    }
    
    free(cadena);
    fclose(fp);
    return 0;
}