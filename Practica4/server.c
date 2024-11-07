#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>
#include<sys/types.h>
#include <pthread.h>

int *semid;
int *clientId;
char (*frase);

int CreaSemaforo( key_t llave, int valor_inicial){
    int semid= semget(llave, 1, IPC_CREAT|0644);
    if(semid==-1){
        perror("Error al crear el semaforo");
        exit(1);
    }
    semctl (semid, 0, SETVAL, valor_inicial);
    return semid;
}

void* AttendClient(void* args){
    printf("hilo...");
}

void CreateCriticalZone(int semaforo){
    pthread_t th;
    key_t keyFrase, keyClientId, keySemId, keySemValue;
    int shmBuffidfrase, shmBuffidclientid,shmBuffidSemId;

    keyFrase = ftok("frase", 'p');
    keyClientId = ftok("clienteid", 'p');
    keySemId = ftok("semid", 'p');

    shmBuffidfrase = shmget(keyFrase, (100*sizeof(char)), IPC_CREAT|0777);
    shmBuffidclientid = shmget(keyClientId, sizeof(int), IPC_CREAT|0777);
    shmBuffidSemId = shmget(keySemId, sizeof(int), IPC_CREAT|0777);

    if( shmBuffidclientid == -1 || shmBuffidfrase == -1 ){
        perror("Error 1 al reservar la memoria compartida");
        exit(1);
    }else{
        frase = shmat(shmBuffidfrase, 0, 0);
        clientId = shmat(shmBuffidclientid, 0, 0);
        semid = shmat(shmBuffidSemId,0,0);
        *clientId = 0;
        *semid = semaforo;
        if( frase == (void *)-1 || clientId == (void *)-1 ){
            perror("Error 2 al reservar la memoria compartida");
            exit(1);
        }
        while(1){
            printf("%d", semctl(semaforo, 0, GETVAL));
            if( *clientId != 0 ){
                printf("Atenediendo al cliente %d\n", *clientId);
                if( pthread_create(&th, NULL, &AttendClient, NULL) != 0 ){
                    perror("Erro al crar el hilo\n");
                }
                sleep(1);
            }else{
                printf("Waiting...\n");
                sleep(1);
            }
        }

    }
}

int main(int argc, int* argv[]){
    key_t llave_semaforo;
    int semaforo_mutex;

    llave_semaforo = ftok("semid", 'p');
    semaforo_mutex = CreaSemaforo(llave_semaforo, 1);
    CreateCriticalZone(semaforo_mutex);
    return 0;
}