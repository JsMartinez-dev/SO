#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

// El mutex protege la variable 'comida_lista'
pthread_mutex_t mutex_mostrador = PTHREAD_MUTEX_INITIALIZER;

// La variable de condición para "avisar"
pthread_cond_t cond_comida_lista = PTHREAD_COND_INITIALIZER;

// La condición que comprobamos (el recurso compartido)
int comida_lista = 0; 

void* funcion_cocinero(void* arg) {
    printf("Cocinero: Cocinando... (tarda 3 seg)\n");
    sleep(3);
    
    // 1. Cocinero coge el mutex para poner la comida
    pthread_mutex_lock(&mutex_mostrador);
    
    printf("Cocinero: ¡Pedido listo en el mostrador!\n");
    comida_lista = 1; // Pone la comida
    
    // 2. Avisa al camarero que esté esperando (signal)
    pthread_cond_signal(&cond_comida_lista);
    
    // 3. Suelta el mutex
    pthread_mutex_unlock(&mutex_mostrador);
    return NULL;
}

void* funcion_camarero(void* arg) {
    printf("Camarero: Esperando pedido...\n");
    
    // 1. Camarero coge el mutex para ver el mostrador
    pthread_mutex_lock(&mutex_mostrador);
    
    // 2. Comprueba la condición en un 'while'
    // Si no hay comida (comida_lista == 0), espera.
    while (comida_lista == 0) {
        // 3. Magia: 'wait' DUERME al hilo Y SUELTA EL MUTEX
        // (para que el cocinero pueda cogerlo)
        pthread_cond_wait(&cond_comida_lista, &mutex_mostrador);
        // Cuando despierta, AUTOMÁTICAMENTE recupera el mutex
    }
    
    // 4. Si sale del 'while', es porque comida_lista == 1
    printf("Camarero: ¡Recogiendo pedido! Gracias.\n");
    comida_lista = 0; // Se lleva la comida
    
    // 5. Suelta el mutex
    pthread_mutex_unlock(&mutex_mostrador);
    return NULL;
}

int main() {
    pthread_t cocinero, camarero;
    
    pthread_create(&camarero, NULL, funcion_camarero, NULL);
    pthread_create(&cocinero, NULL, funcion_cocinero, NULL);
    
    pthread_join(cocinero, NULL);
    pthread_join(camarero, NULL);
    
    // (Destruir mutex y cond)
    
    return 0;
}