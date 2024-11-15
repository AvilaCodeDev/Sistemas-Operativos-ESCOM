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

void servicio()
{
   int proceso,id_usuario;
   char nombre_usuario[20],contrasena[20];
   struct persona
   {
      char user[20];
      char password[20];
      int pid;
   } *usuario;
   key_t llave_usuario;
   llave_usuario=ftok("Prueba_usuario",'u');
   id_usuario=shmget(llave_usuario,sizeof(struct persona),IPC_CREAT|0777);
   usuario=(struct persona *)shmat(id_usuario,0,0);
   proceso=usuario->pid;
   strcpy(nombre_usuario,usuario->user);
   strcpy(contrasena,usuario->password);
   while(1)
   {
      printf("\nAtendiendo al cliente %d...\n",proceso);
      printf("\nRecibiendo usuario: %s\n",nombre_usuario);
      printf("\nRecibiendo contrasena: %s\n",contrasena);
      //Para ver los hilos -> pstree -p PID
      //También -> ps -p PID -L
      sleep(1);
   }
   pthread_exit(NULL);
}

int main()
{
   int mutex, clientes, i;
   key_t llave_mutex,llave_clientes,llave_pid;
   pthread_t id_hilo;
   pthread_attr_t atributos;
   llave_mutex=ftok("Prueba_mutex",'s');
   llave_clientes=ftok("Prueba_clientes",'c');
   clientes=crea_semaforo(llave_clientes,0);
   mutex=crea_semaforo(llave_mutex,1);
   pthread_attr_init(&atributos);
   pthread_attr_setdetachstate(&atributos,PTHREAD_CREATE_DETACHED);
   
   while(1)
   {
      printf("Servidor con pid %d esperando cliente...\n",getpid());
      down(clientes);
      down(mutex);
      printf("Se ha atendido el cliente numero %d...\n",i++);
      pthread_create(&id_hilo,&atributos,(void*)servicio,NULL);
   }
}