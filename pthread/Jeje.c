// planificador.c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int *slots;              // arreglo de W casillas: -1 libre, >=0 duración asignada
int W;
int *tasks;              // vector de duraciones
int n_tasks = 0;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int tasks_assigned = 0;
int done = 0;

void *worker_fn(void *arg) {
    int id = *(int *)arg;
    free(arg);

    while (1) {
        pthread_mutex_lock(&mtx);
        // Espera hasta que su casilla sea distinta de -1 o hasta done
        while (slots[id] == -1 && !done) {
            pthread_cond_wait(&cond, &mtx);
        }

        if (slots[id] == -1 && done) {
            // No hay más trabajo y estamos terminando
            pthread_mutex_unlock(&mtx);
            break;
        }

        // Hay una tarea en su casilla
        int dur = slots[id];
        // Marca la casilla con -2 para indicar que la está ejecutando (opcional)
        slots[id] = -2;
        pthread_mutex_unlock(&mtx);

        printf("(hilo %d) realizando tarea (%d)\n", id+1, dur);
        // Simular ejecución de la tarea: sleep en segundos
        sleep(dur);

        // Al terminar, marcar casilla como libre (-1) y notificar al dispatcher
        pthread_mutex_lock(&mtx);
        slots[id] = -1;
        printf("(hilo %d) terminó tarea (%d), libera núcleo\n", id+1, dur);
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mtx);
    }

    printf("(hilo %d) saliendo\n", id+1);
    return NULL;
}

void *dispatcher_fn(void *arg) {
    (void)arg;
    int task_idx = 0;
    int pos = 0; // posición actual en arreglo de núcleos

    while (1) {
        pthread_mutex_lock(&mtx);
        // Si ya asigné todas las tareas, salgo del bucle y espero que todos terminen
        if (task_idx >= n_tasks) {
            // Esperar hasta que todas las casillas estén libres
            int all_free = 1;
            for (int i = 0; i < W; ++i) {
                if (slots[i] != -1) { all_free = 0; break; }
            }
            if (all_free) {
                // Indicar fin a los trabajadores
                done = 1;
                pthread_cond_broadcast(&cond);
                pthread_mutex_unlock(&mtx);
                break;
            }
            // Si no todas libres, esperar que trabajadores liberen
            pthread_cond_wait(&cond, &mtx);
            pthread_mutex_unlock(&mtx);
            continue;
        }

        // Buscar la siguiente casilla libre (circular) y asignar la tarea
        int found = 0;
        for (int i = 0; i < W; ++i) {
            int idx = (pos + i) % W;
            if (slots[idx] == -1) {
                // asignar tarea task_idx a casilla idx
                slots[idx] = tasks[task_idx];
                tasks_assigned++;
                printf("Distribuidor (asignando tareas a hilo %d) (%d)\n", idx+1, tasks[task_idx]);
                task_idx++;
                pos = (idx + 1) % W;
                found = 1;
                // Notificar a trabajadores que puede haber trabajo
                pthread_cond_broadcast(&cond);
                break;
            }
        }

        if (!found) {
            // No hay casilla libre ahora; esperar a que algún trabajador termine
            pthread_cond_wait(&cond, &mtx);
        }
        pthread_mutex_unlock(&mtx);
    }

    printf("Distribuidor: no quedan más tareas, finalizando\n");
    return NULL;
}

int read_input(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return -1;

    // Leer W
    if (fscanf(f, "%d", &W) != 1) {
        fclose(f);
        return -1;
    }
    if (W <= 0) {
        fclose(f);
        return -1;
    }

    // Leer duraciones (hasta EOF). Primero contar
    int cap = 32;
    tasks = malloc(cap * sizeof(int));
    if (!tasks) { fclose(f); return -1; }

    int t;
    while (fscanf(f, "%d", &t) == 1) {
        if (t < 0) t = 0;
        if (n_tasks >= cap) {
            cap *= 2;
            tasks = realloc(tasks, cap * sizeof(int));
            if (!tasks) { fclose(f); return -1; }
        }
        tasks[n_tasks++] = t;
    }

    fclose(f);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s archivo_tareas\n", argv[0]);
        fprintf(stderr, "Formato: primera linea W (num hilos), luego una duración por línea\n");
        return 1;
    }

    if (read_input(argv[1]) != 0) {
        fprintf(stderr, "Error leyendo archivo %s\n", argv[1]);
        return 1;
    }

    if (n_tasks == 0) {
        printf("No hay tareas en el archivo. Saliendo.\n");
        free(tasks);
        return 0;
    }

    // Inicializar slots
    slots = malloc(W * sizeof(int));
    for (int i = 0; i < W; ++i) slots[i] = -1;

    pthread_t dispatcher;
    pthread_t *workers = malloc(W * sizeof(pthread_t));

    // Crear hilos trabajadores
    for (int i = 0; i < W; ++i) {
        int *id = malloc(sizeof(int));
        *id = i;
        if (pthread_create(&workers[i], NULL, worker_fn, id) != 0) {
            perror("pthread_create worker");
            return 1;
        }
    }

    // Crear dispatcher
    if (pthread_create(&dispatcher, NULL, dispatcher_fn, NULL) != 0) {
        perror("pthread_create dispatcher");
        return 1;
    }

    // Esperar al dispatcher
    pthread_join(dispatcher, NULL);

    // Esperar a los trabajadores
    for (int i = 0; i < W; ++i) {
        pthread_join(workers[i], NULL);
    }

    // Limpieza
    free(workers);
    free(slots);
    free(tasks);
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&cond);

    printf("Planificador finalizado.\n");
    return 0;
}
