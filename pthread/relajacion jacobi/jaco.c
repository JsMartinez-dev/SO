#define _POSIX_C_SOURCE 200809L // Habilita las definiciones POSIX (incluyendo barreras)

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

float** A = NULL;
float** B = NULL;

int rows = 0;
int cols = 0;
int n_days;

int ready = 0;
int should_print = 0;
int should_exit = 0;
int n_t = 0;

pthread_barrier_t barrier;

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx_c = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

typedef struct args {
    int start;
    int end;
} Args;

void swap(){
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            A[i][j] = B[i][j];
        }
    }
}

void read_file(const char* fname){

    FILE* f = fopen(fname, "r");
    if(!f){
        perror("file");
        abort();
    }

    fscanf(f, "%d %d", &rows, &cols);

    A = (float**)malloc(sizeof(float*)*rows);
    B = (float**)malloc(sizeof(float*)*rows);
    if(!A || !B){
        perror("memory");
        abort();
    }

    for(int i = 0; i < rows; i++){
        A[i] = (float*)malloc(sizeof(float)*cols);
        B[i] = (float*)malloc(sizeof(float)*cols);
        if(!A[i] || !B[i]){
            perror("memory");
            abort();
        }
    }

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            fscanf(f, "%f", &A[i][j]);
            B[i][j] = 0;
        }
    }
}


void print_matriz(float**M, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++){
            printf("%.1f ", M[i][j]);
        }
        printf("\n");
    }
}

float mean(int i, int j){
    return (A[i-1][j] + A[i+1][j] + A[i][j-1] + A[i][j+1])/4;
}

void* jacobi_relax(void* args){

    Args* arg = (Args*)args;
    int c = 0;

    do{

        for(int i = arg->start; i < arg->end; i++){
            for(int j = 1; j < cols - 1; j++){
                //printf("%f\n", B[i][j]);
                B[i][j] = mean(i, j);
            }
            //printf("\n");
        }

        c++;

        pthread_mutex_lock(&mtx);
        ready++;
        pthread_mutex_unlock(&mtx);

        if(ready == n_t-1){
            swap();
            ready = 0;
            pthread_cond_signal(&cond);
        }

        if(c == n_days){
            should_exit = 1;
            pthread_cond_signal(&cond);
            return NULL;
        }

        pthread_barrier_wait(&barrier);

    }while(c < n_days);
    return NULL;
}

int main(int argc, char**argv){

    if(argc < 4){
        printf("usage: ./a.out [n threads] [n days] file\n");
        return 1;
    }

    read_file(argv[3]);
    //print_matriz(A, rows, cols);

    n_t = atoi(argv[1]);
    n_days = atoi(argv[2]);


    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t)*n_t);
    if(!threads){
        perror("thread vec");
        return 1;
    }

    pthread_barrier_init(&barrier,NULL,n_t);

    int n_rows = rows - 2;
    int delt = (rows / n_t);

    for(int i = 0; i < n_t; i++){
        Args* arg = (Args*)malloc(sizeof(Args));
        arg->start = i*delt + 1;
        arg->end = (i == n_t-1) ? rows-1 : arg->start + delt;
        pthread_create(&threads[i], NULL, jacobi_relax, (void*)arg);
    }

    while(1){

        pthread_mutex_lock(&mtx_c);
        while(should_print != 0){
            pthread_cond_wait(&cond, &mtx_c);
        }
        pthread_mutex_unlock(&mtx_c);
        print_matriz(B, rows, cols);     
        printf("----------------------------------------------------------\n");
        if(should_exit){
            break;
        }
    }

    for(int i = 0; i < n_t; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barrier);

    return 0;
}