#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include "mainn.h"

Data *threadInfo = NULL;
int *numbers = NULL;

pthread_cond_t queueEmpty ;
pthread_mutex_t queueLock ;

void * Slave_Start(void * s)
{
  int id = (*(Data *) s).id ;
  int min = (*(Data *) s).min;
  int max = (*(Data *) s).max;
  int k = (*(Data *) s).k;
  pthread_cond_t noWork = (*(Data *) s).noWork;
  pthread_mutex_t infoLock = (*(Data *) s).infoLock;
  
  printf("Slave Started %i\n", id);

  if (min == -1)
  {
    pthread_cond_init(&noWork, NULL);
    pthread_mutex_init(&infoLock, NULL);

    pthread_mutex_lock(&queueLock);
    Enqueue(id);
    pthread_mutex_unlock(&queueLock);
    pthread_cond_signal(&queueEmpty);

    pthread_mutex_lock(&infoLock);
    while ((*(Data *) s).min == -1)
      pthread_cond_wait(&noWork, &infoLock);
    pthread_mutex_unlock(&infoLock);
  }

  k = min % k;
  
    
}

void init()
{
  pthread_cond_init(&queueEmpty, NULL);
  pthread_mutex_init(&queueLock, NULL);
}

int main (int argc, char *argv [])
{
  int n = atoi( argv[1] );
  int numSlaves = atoi( argv[2] );
  int c = atoi( argv[3] );  
  pthread_t thread;
  int i, k, maxK, range, numChunks, mod, modH, id;
  
  threadInfo = malloc(sizeof(Data)*numSlaves);
  numbers = malloc(sizeof(int)*(n + 1));

  for (i=0; i<numSlaves; i++)
  {
    threadInfo[i].id = i;   
    threadInfo[i].min = -1;

    if(pthread_create(&thread, NULL, Slave_Start, &threadInfo[i])!=0) 
    {
      perror("Student thread could not be created!\n");
      exit(0);
    }
  }

  maxK = floor(sqrt(n));

  for (k=2; k <= maxK; k++)
  {
    if(numbers[k] == 1)
      continue;

    range = n - k + 1;
    numChunks = range / c;
    mod = 0;
    modH = 0;
      
    mod = range % c;

    for (i=0; i < numChunks; i++)
    {
      pthread_mutex_lock(&queueLock);
      
      while (front == NULL)
        pthread_cond_wait(&queueEmpty, &queueLock) ;

      id = front->data;
      Dequeue();

      pthread_mutex_unlock(&queueLock);

      pthread_mutex_lock(&(threadInfo[id].infoLock));

      if (mod == 0 || i < mod)
      {
        threadInfo[id].min = (i * c) + 2;
        threadInfo[id].max = (i * c) + 1 + c;
      }
      else
      {
        threadInfo[id].min = (i * c) + 2 - modH;
        threadInfo[id].max = (i * c) + c - modH;
        modH++; 
      }
      
      pthread_mutex_unlock(&(threadInfo[id].infoLock));
      pthread_cond_signal(&(threadInfo[id].noWork));    
    }
  
  }

  while(1);
}
