#define _POSIX_C_SOURCE 200809L // Habilita las definiciones POSIX (incluyendo barreras)

#include <stdio.h>
#include <pthread.h>      // Contiene pthread_barrier_t
#include <unistd.h>
// ... el resto de tu código
// 1. Declaramos la barrera
pthread_barrier_t mi_barrera;

void* funcion_hilo(void* arg) {
    long id = (long)arg;
    
    // --- FASE 1 ---
    printf("Hilo %ld: Haciendo Fase 1 (tarda %ld seg)\n", id, id);
    sleep(id); // Cada hilo tarda un tiempo diferente
    
    printf("Hilo %ld: LLEGÓ a la barrera.\n", id);
    
    // 3. El hilo espera en la barrera
    // Se bloqueará aquí hasta que 3 hilos (ver main) lleguen.
    pthread_barrier_wait(&mi_barrera);
    
    // --- FASE 2 ---
    // Este código solo se ejecuta DESPUÉS de que todos lleguen
    printf("Hilo %ld: Barrera rota. ¡Empezando Fase 2!\n", id);
    
    return NULL;
}

int main() {
    int num_hilos = 3;
    pthread_t hilos[num_hilos];
    
    // 2. Inicializamos la barrera
    // NULL = atributos por defecto
    // num_hilos = N° de hilos que deben esperar
    pthread_barrier_init(&mi_barrera, NULL, num_hilos);
    
    // Creamos 3 hilos (id 1, 2, 3 para que tarden distinto)
    for (long i = 1; i <= num_hilos; i++) {
        pthread_create(&hilos[i-1], NULL, funcion_hilo, (void*)i);
    }
    
    // Esperamos a que todos terminen
    for (int i = 0; i < num_hilos; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    // 4. Destruimos la barrera
    pthread_barrier_destroy(&mi_barrera);
    
    return 0;
}