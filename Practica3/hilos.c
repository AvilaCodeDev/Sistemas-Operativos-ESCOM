#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

pthread_mutex_t mutexMatrix;
int mtx[3][9];

void MuestraMatriz(){
    int i = 0, j = 0, value;

    for( i=0; i<3; i++ ){
        for( j=0; j<9; j++ ){
            printf("%d ",mtx[i][j]);
        }
        printf("\n");
    }
}

void MuestraFilaMatriz(int fila ){
    int i;
    for( i = 0; i < 9; i++ ){
        printf("%d ", mtx[fila][i] );
    }
    printf("\n");
}

void LlenaMatriz(){
    int i = 0, j = 0, value;

    for( i=0; i<3; i++ ){
        for( j=0; j<9; j++ ){
            if(i == 0) value = j+1;
            if(i == 1) value = ((j+1)*2)-1;
            if(i == 2) value = (j+1)*2; 
            mtx[i][j] = value;
        }
    }
}



void MuestraMensaje(int fila, int operacion){
    if( operacion == 1)printf("Hola, soy el hilo: %ld, y voy a sumar la fila con el indice %d,\n", pthread_self(), fila);
    if( operacion == 2)printf("Hola, soy el hilo: %ld, y voy a multiplicar la fila con el indice %d,\n", pthread_self(), fila);
    printf("que contiene los siguientes elementos: \n");
    MuestraFilaMatriz(fila);
    // sleep(1);
}

void* Suma(void * arg){
    int fila = *(int*)arg;
    int sum = 0;
    MuestraMensaje(fila, 1);  
    pthread_mutex_lock(&mutexMatrix);
    for(int i = 0; i < 9; i++ ){
        sum += mtx[fila][i];
    }
    *(int*)arg = sum;
    pthread_mutex_unlock(&mutexMatrix);
    return arg;
}

void* Multiplicacion(void* arg){
    int fila = *(int*)arg;
    int mult = 1;
    MuestraMensaje(fila, 2);
    pthread_mutex_lock(&mutexMatrix);
    for(int i = 0; i < 9; i++ ){
        mult *= mtx[fila][i];
    }
    *(int*)arg = mult;
    pthread_mutex_unlock(&mutexMatrix);
    return arg;
}

int main( int argc, int* argv[] ){
    pthread_t th[3];
    int opc_operacion = 1;

    pthread_mutex_init(&mutexMatrix, NULL);
    LlenaMatriz();
    MuestraMatriz();

    while(opc_operacion < 3){
        for( int i = 0; i < 3; i++ ){
            int* a = malloc(sizeof(int));
            *a = i;
            if(opc_operacion == 1){
                if( pthread_create(&th[i], NULL, &Suma, a) != 0 ){
                    perror("Erro al crar el hilo\n");
                }
            }
            if(opc_operacion == 2){
                if( pthread_create(&th[i], NULL, &Multiplicacion, a) != 0 ){
                    perror("Erro al crar el hilo\n");
                }
            }

        }

        for( int j = 0; j < 3; j++ ){
            int* result;
            if(pthread_join(th[j], (void*)&result) != 0){
                perror("Error al hacer el join de los hilos.\n");
            }
            if(opc_operacion == 1 ) printf("La suma realizada en la fila con el indice %d ", j);
            if(opc_operacion == 2 ) printf("La multiplicacion realizada en la fila con el indice %d ", j);
            printf("dio como resultado: %d\n\n", *result);
            free(result);
        }
        opc_operacion++;
        sleep(3);
    }
    pthread_mutex_destroy(&mutexMatrix);
    return 0;
}