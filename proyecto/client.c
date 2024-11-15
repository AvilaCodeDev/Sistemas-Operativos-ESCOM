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
   int clientes,id_usuario,mutex;
   struct persona
   {
      char user[20];
      char password[20];
      int pid;
   } *usuario;
   key_t llave_clientes,llave_usuario,llave_mutex;
   llave_clientes=ftok("Prueba_clientes",'c');
   llave_usuario=ftok("Prueba_usuario",'u');
   llave_mutex=ftok("Prueba_mutex",'s');
   clientes=crea_semaforo(llave_clientes,0);
   mutex=crea_semaforo(llave_mutex,0);
   id_usuario=shmget(llave_usuario,sizeof(struct persona),IPC_CREAT|0777);
   usuario=(struct persona *)shmat(id_usuario,0,0);
   up(clientes);
   while(1)
   {
       system("clear");
       usuario->pid=getpid();
       printf("\nSoy el proceso: %d\n",usuario->pid);
       printf("\nEscribe nombre de usuario: ");
       scanf("%s",usuario->user);
       printf("\nEscribe contrasena: ");
       scanf("%s",usuario->password);
       up(mutex);
   }
}