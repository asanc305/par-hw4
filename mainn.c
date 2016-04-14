#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
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
  int i;
  pthread_cond_t noWork;
  pthread_mutex_t infoLock; 

  pthread_cond_init(&noWork, NULL);
  pthread_mutex_init(&infoLock, NULL);
  
  (*(Data *) s).noWork = noWork;
	(*(Data *) s).infoLock = infoLock;
	
	while(1)
	{	
	  pthread_mutex_lock(&queueLock);
		Enqueue(id);
		pthread_mutex_unlock(&queueLock);
		pthread_cond_signal(&queueEmpty);
		
		id = (*(Data *) s).id ;
		min = (*(Data *) s).min;
		max = (*(Data *) s).max;
		k = (*(Data *) s).k;
		
		pthread_mutex_lock(&infoLock);
		(*(Data *) s).min = -111;
  	printf("[SLAVE] waiting %i\n", id);
    pthread_cond_wait(&noWork, &infoLock);
  	pthread_mutex_unlock(&infoLock);
  	
  	printf("[SLAVE] started %i\n", id);
  	if ((min % k) != 0)
  		min = min - (min % k) + k;
  	
  	for (i=min; i<=max; i=i+k)
		{
  		numbers[i] = 1;
		}
		
		(*(Data *) s).min = -111;
		printf("[SLAVE] ended %i\n", id);
	}	
    
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

	sleep(10);
  maxK = floor(sqrt(n));

  for (k=2; k <= maxK; k++)
  {
    if(numbers[k] == 1)
      continue;

    range = n - k + 1;
    numChunks = range / c;
    mod = 0;
    modH = 0;
      
    mod = range % (numChunks+1);

    for (i=0; i <= numChunks; i++)
    {
      pthread_mutex_lock(&queueLock);
      
      while (front == NULL)
        pthread_cond_wait(&queueEmpty, &queueLock) ;

      id = front->data;
      Dequeue();

      pthread_mutex_unlock(&queueLock);

      pthread_mutex_lock(&(threadInfo[id].infoLock));
      
      printf("MIN = %i\n", threadInfo[id].min);

      if (mod == 0 || i < mod)
      {
        threadInfo[id].min = (i * c) + 2;
        int er = (i*c)+2;
        threadInfo[id].max = (i * c) + 1 + c;
      }
      else
      {
        threadInfo[id].min = (i * c) + 2 - modH;
        threadInfo[id].max = (i * c) + c - modH;
        modH++; 
      }
      
      printf("[MASTER] Slave started %i\n", id); 
      threadInfo[id].k = k;
      pthread_mutex_unlock(&(threadInfo[id].infoLock));
      pthread_cond_signal(&(threadInfo[id].noWork));   
    }
  }

  if (numbers[n] == 0)
  	printf("Its prime\n");
	else
		printf("Its not prime\n");
}
