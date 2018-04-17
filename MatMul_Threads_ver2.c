#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <malloc.h>

#include <pthread.h>

void Display(int dim, float *mat);
void Fill(int size, float *data);
void MatMul(int dim, float *A, float *B, float *Product);
void MatMul_MT(int dim, float *A, float *B, float *Product, int noThread);

int main(int argc, char *argv[])
{
    int dim = 10;
    int noThread = 10;

    struct timeval t1, t2;
    float elapsed_time = 0.F;
    
    if(argc < 2){
        printf("Usage: %s <dim> <no_threads>\n", argv[0]);
        exit(0);
    }

    dim = atoi(argv[1]);
    if(dim > 1000)
        dim = 1000;
    noThread = atoi(argv[2]);

    srand(time(NULL));
    
    float *A = (float*)malloc(dim * dim * sizeof(float));
    float *B = (float*)malloc(dim * dim * sizeof(float));
    float *Product = (float*)malloc(dim * dim * sizeof(float));

    if(A == NULL || B == NULL || Product == NULL){
        printf("Failed to allocate memory.\n");
        exit(-1);
    }

    Fill(dim * dim, A);
    Fill(dim * dim, B);
    
    if(dim <= 10){
        printf("A = \n");
        Display(dim, A);
    
        printf("B = \n");
        Display(dim, B);
    }

    gettimeofday(&t1, NULL);
    
    MatMul(dim, A, B, Product);
    
    gettimeofday(&t2, NULL);
    elapsed_time = (t2.tv_sec + t2.tv_usec / 1000000.) - (t1.tv_sec + t1.tv_usec / 1000000.);
    printf("elapsed time (single thread)\t= %15f sec\n", elapsed_time);

    if(dim <= 10){
        printf("A * B (single thread) = \n");
        Display(dim, Product);
    }

    gettimeofday(&t1, NULL);

    MatMul_MT(dim, A, B, Product, noThread);

    gettimeofday(&t2, NULL);
    elapsed_time = (t2.tv_sec + t2.tv_usec / 1000000.) - (t1.tv_sec + t1.tv_usec / 1000000.);
    printf("elapsed time (%d threads)\t= %15f sec\n", noThread, elapsed_time);

    if(dim <= 10){
        printf("A * B (%d threads) = \n", noThread);
        Display(dim, Product);
    }


    free(A);
    free(B);
    free(Product);

    return 0;
}

void Display(int dim, float *mat)
{
    if(dim >= 10)
        return;
    for(int i = 0; i < dim; i++){
        for(int j = 0; j < dim; j++)
            printf("%.2f ", mat[i * dim + j]);
        printf("\n");
    }
}

void Fill(int size, float *data)
{
    int i = 0;
    for(i = 0; i < size; i++)
        data[i] = i;
}


void MatMul(int dim, float *A, float *B, float *Product)
{
    int i = 0, j = 0, k = 0;

    for(i = 0; i < dim; i++){
        for(j = 0; j < dim; j++){
            Product[i * dim + j] = 0.F;
            for(k = 0; k < dim; k++)
                Product[i * dim + j] += A[i * dim + k] * B[k * dim + j];
        }
    }
}

typedef struct{
    int id; //thread index
    int noThread;
    int dim;
    float *A, *B, *Product;
} ThreadInfo;

void* MatMul_ThreadFn(void *param);


void MatMul_MT(int dim, float *A, float *B, float *Product, int noThread)
{
    int i = 0;
    pthread_t * tid = (pthread_t*)malloc(sizeof(pthread_t) * noThread);
    ThreadInfo * threadInfo = (ThreadInfo*)malloc(sizeof(ThreadInfo) * noThread);
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    for(i = 0; i < noThread; i++){// Repeat for noThread times
        threadInfo[i].id = i;
        threadInfo[i].noThread = noThread;
        threadInfo[i].dim = dim;
        threadInfo[i].A = A;
        threadInfo[i].B = B;
        threadInfo[i].Product = Product;
        
        //pthread_attr_init(&attr);
        pthread_create(&tid[i], &attr, MatMul_ThreadFn, &threadInfo[i]);
    }
         
    for(i = 0; i < noThread; i++)
        pthread_join(tid[i], NULL);// wait for the children threads to terminate

    free(tid);
    free(threadInfo);// deallocate pthread_t and ThreadInfo array
}

void* MatMul_ThreadFn(void *param)      // thread function
{
    ThreadInfo *pInfo = (ThreadInfo *)param;
    int i = 0, j = 0, k = 0;
    int start = pInfo->id;
    int end = pInfo->dim;
    int noThread = pInfo->noThread;

    /*i-th thread computes only (i + k * noThread)-th rows*/
    for(i = pInfo->id; i < end; i+=noThread){
        for(j = 0; j < pInfo->dim; j++){
            pInfo->Product[i * pInfo->dim + j] = 0.F;
            for(k = 0; k < pInfo->dim; k++){
                pInfo->Product[i * pInfo->dim + j] += pInfo->A[i * pInfo->dim + k] * pInfo->B[k * pInfo->dim + j]; // multiply two matrics pInfo->A and pInfo->B put the Product in pInfo->Product
            }
        }
    }
    pthread_exit(0);
}


