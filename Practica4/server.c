#include<sys/types.h>
#include<sys/shm.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<string.h>

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

void AtiendeCliente(){
   int id_cliente;
   char str[1024];
   struct client{
      int pid;
      char frase[100];
   } *cliente;
   key_t llave_cliente;
   llave_cliente = ftok("archivo_clientes", 'c');
   id_cliente = shmget(llave_cliente, sizeof(struct client), IPC_CREAT|0777);
   cliente = (struct client *)shmat(id_cliente,0,0);
   printf("Atendiendo al ciente: %d\n", cliente->pid);
   puts(cliente->frase);
   pthread_exit(NULL);
}

int main()
{
   int mutex;
   key_t llave_mutex;
   pthread_t id_hilo;
   pthread_attr_t atributos;
   llave_mutex=ftok("semaforo_mutex",'s');
   mutex=crea_semaforo(llave_mutex,0);
   pthread_attr_init(&atributos);
   pthread_attr_setdetachstate(&atributos,PTHREAD_CREATE_DETACHED);
   
   while(1)
   {
      printf("Servidor con pid %d esperando cliente...\n",getpid());
      down(mutex);
      pthread_create(&id_hilo,&atributos,(void*)AtiendeCliente,NULL);
   }
}
