#include <stdio.h>
#include <pthread.h>
#include <semaphore.h> // Necesario para semáforos
#include <unistd.h>     // Para sleep()

// 1. Declaramos el "portero" (semáforo)
sem_t semaforo_discoteca;

void* funcion_hilo(void* arg) {
    long id = (long)arg; // ID del hilo (para saber quién es)
    
    printf("Hilo %ld: Esperando en la cola...\n", id);
    
    // 3. El hilo intenta entrar (wait/P)
    // Si el contador del semáforo es 0, se bloquea aquí.
    sem_wait(&semaforo_discoteca);
    
    // --- SECCIÓN CRÍTICA (DENTRO DE LA DISCOTECA) ---
    printf("Hilo %ld: ¡ENTRÓ! (Quedan menos sitios)\n", id);
    sleep(2); // Simula que está "bailando" (usando el recurso)
    printf("Hilo %ld: Saliendo...\n", id);
    // --- FIN SECCIÓN CRÍTICA ---
    
    // 4. El hilo sale (post/V)
    // Libera un sitio, despertando a un hilo en espera.
    sem_post(&semaforo_discoteca);
    
    return NULL;
}

int main() {
    pthread_t hilos[5];
    
    // 2. Inicializamos el semáforo
    // 0 = se comparte entre hilos del mismo proceso
    // 2 = valor inicial (la capacidad de la discoteca)
    sem_init(&semaforo_discoteca, 0, 2);
    
    printf("Discoteca abierta con capacidad para 2.\n");

    // Creamos 5 hilos
    for (long i = 0; i < 5; i++) {
        pthread_create(&hilos[i], NULL, funcion_hilo, (void*)i);
    }
    
    // Esperamos a que todos terminen
    for (int i = 0; i < 5; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // 5. Destruimos el semáforo
    sem_destroy(&semaforo_discoteca);
    
    return 0;
}