#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/sem.h>

int *semid;
int *clientId;
char (*frase);

void down(int semid){
   struct sembuf op_p[]={0,-1,0};
   semop(semid,op_p,1);
}

void up(int semid){
   struct sembuf op_v[]={0,+1,0};
   semop(semid,op_v,1);
}

void WriteMessage(){
    int i = 0;
    frase = "Hola, es mi turno de comunicarme con el servidor";

    while(frase[i] != '\0'){
        printf("%c", frase[i]);
        i++;
    }
}

void AccesCriticalZone(){
    key_t keyFrase, keyClientId;
    int shmBuffidfrase, shmBuffidclientid;

    keyFrase = ftok("frase", 'p');
    keyClientId = ftok("clienteid", 'p');
    

    shmBuffidclientid = shmget(keyClientId, sizeof(int), IPC_CREAT|0777);
    shmBuffidfrase = shmget(keyFrase, 100*sizeof(char), IPC_CREAT|0777);
    

    if(  shmBuffidclientid == -1 || shmBuffidfrase == -1  ){
        perror("Error al reservar la memoria compartida");
        exit(1);
    }else{
        frase = shmat(shmBuffidfrase, 0, 0);
        clientId = shmat(shmBuffidclientid, 0, 0);
        if( frase == (void *)-1 || clientId == (void *)-1 ){
            perror("Error al reservar la memoria compartida");
            exit(1);
        }
        printf("Hola, soy el cliente %d\n", getpid());
        *clientId = getpid();
        WriteMessage();
        sleep(3);
    }

    *clientId = 0;

}

int main( int argc, int* argv[] ){
    key_t keySemId;
    int shmBuffidSemId;

    keySemId = ftok("semid", 'p');
    shmBuffidSemId = shmget(keySemId, sizeof(int), IPC_CREAT|0777);
    semid = shmat(shmBuffidSemId,0,0);

    while(semctl(*semid, 0, GETVAL) <= 0){
        printf("Waiting...\n");
        sleep(3);
    }
    down(*semid);
    printf("%d", semctl(*semid, 0, GETVAL));
    AccesCriticalZone();
    up(*semid);
    return 0;
}