#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "mainn.h"

int *numbers = NULL;

pthread_cond_t queueEmpty ;
pthread_mutex_t queueLock ;

void * Slave_Start(void * s)
{
  int id = (*(struct Data *) s).id ; ;
  int min = (*(struct Data *) s).min;
  int max = (*(struct Data *) s).max;
  int k = (*(struct Data *) s).k;
  int i;
  pthread_cond_t *noWork = (*(struct Data *) s).noWork;
  pthread_mutex_t *infoLock = (*(struct Data *) s).infoLock;
  
	while(1)
	{	
	  pthread_mutex_lock(&queueLock);
		Enqueue(id);
		pthread_mutex_unlock(&queueLock);
		pthread_cond_signal(&queueEmpty);
		
		pthread_mutex_lock(infoLock);
    pthread_cond_wait(noWork, infoLock);
  	pthread_mutex_unlock(infoLock);
  	
  	min = (*(struct Data *) s).min;
		max = (*(struct Data *) s).max;
		k = (*(struct Data *) s).k;
  	
  	if ((min % k) != 0)
  		min = min - (min % k) + k;
  	
  	for (i=min; i<=max; i=i+k)
		{
  		numbers[i] = 1;
		}
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
  struct Data *threadInfo;
  int i, k, maxK, range, numChunks, mod, modH, id;
  
  threadInfo = (struct Data*)malloc(sizeof(struct Data)*numSlaves);
  numbers = malloc(sizeof(int)*(n + 1));

  for (i=0; i<numSlaves; i++)
  {
  	pthread_cond_t noWork;
  	pthread_mutex_t infoLock;
  	
    pthread_cond_init(&noWork, NULL);
    pthread_mutex_init(&infoLock, NULL);
  	
    threadInfo[i].id = i;   
    threadInfo[i].min = -1;
    threadInfo[i].noWork = &noWork;
    threadInfo[i].infoLock = &infoLock;

    if(pthread_create(&thread, NULL, Slave_Start, &threadInfo[i])!=0) 
    {
      perror("Student thread could not be created!\n");
      exit(0);
    }
  }

	sleep(3);
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

      pthread_mutex_lock(threadInfo[id].infoLock);
      
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
      
      threadInfo[id].k = k;
      pthread_mutex_unlock((threadInfo[id].infoLock));
      pthread_cond_signal((threadInfo[id].noWork));   
    }
  }

  if (numbers[n] == 0)
  	printf("Prime\n");
	else
		printf("Not Prime\n");
}
