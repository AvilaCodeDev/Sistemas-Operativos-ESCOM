#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int crea_semaforo(key_t llave,int valor_inicial)
{
   int semid=semget(llave,1,IPC_CREAT|0777);
   if(semid==-1)
   {
      return -1;
   }
   semctl(semid,0,SETVAL,valor_inicial);
   return semid;
}

void down(int semid)
{
   struct sembuf op_p[]={0,-1,0};
   semop(semid,op_p,1);
}

void up(int semid)
{
   struct sembuf op_v[]={0,+1,0};
   semop(semid,op_v,1);
}

int main()
{
   struct client{
      int pid;
      char frase[100];
   } *cliente;

   int mutex, id_cliente;
   key_t llave_mutex, llave_cliente;
   llave_mutex=ftok("semaforo_mutex",'s');
   mutex=crea_semaforo(llave_mutex,0);
   llave_cliente=ftok("archivo_clientes", 'c');
   id_cliente = shmget(llave_cliente, sizeof(struct client), IPC_CREAT|0777);
   cliente = (struct client *)shmat(id_cliente,0,0);
    
   system("clear");
   cliente->pid = getpid();
   printf("\nSoy el proceso: %d\n",cliente->pid);
   printf("Ingresa una frase:\n");
   fgets(cliente->frase, 100, stdin);
   up(mutex);
}
