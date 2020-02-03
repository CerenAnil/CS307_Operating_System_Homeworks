#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <queue>
#include <semaphore.h>
using namespace std;

#define NUM_THREADS 5
#define MEMORY_SIZE 1000

struct node
{
	int id;
	int size;
};

///sem_destroy(&)->Sil    sem_post(&)->up sem_wait(&)->Down
queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size

int index = 0;
bool check = true;

void release_function()
{
	//This function will be called
	//whenever the memory is no longer needed.
	//It will kill all the threads and deallocate all the data structures.
  int i = 0;
  while(!myqueue.empty() && i !=5 )
	{
    myqueue.pop();
    sem_destroy(&semlist[i]);
    i++;
  }
}

void my_malloc(int thread_id, int size)
{
	//This function will add the struct to the queue
  node queueNode;
  queueNode.id = thread_id;
  queueNode.size = size;
	pthread_mutex_lock(&sharedLock);
  myqueue.push(queueNode);
	pthread_mutex_unlock(&sharedLock);

}

void * server_function(void *)
{
	//This function should grant or decline a thread depending on memory size.
	while(check)
	{
		pthread_mutex_lock(&sharedLock);
		if(!myqueue.empty())
		{
			if(index + myqueue.front().size > MEMORY_SIZE)
			{
				thread_message[myqueue.front().id] = -1;
			}
			else
			{
				thread_message[myqueue.front().id] = index;
				index += myqueue.front().size;
			}

			sem_post(&semlist[myqueue.front().id]);
			myqueue.pop();
		}
		pthread_mutex_unlock(&sharedLock);
	}

}

void * thread_function(void * id)
{
	//This function will create a random size, and call my_malloc
	//Block
	//Then fill the memory with 1's or give an error prompt
	srand(time(NULL));
	int *intID = (int*)id;
  int randSize = rand()%(MEMORY_SIZE + 1);
  my_malloc(*intID, randSize);

  sem_wait(&semlist[ *intID]);

	if (thread_message[ *intID] == -1)
	{
		pthread_mutex_lock(&sharedLock);
		cout << "Thread" <<  *intID << ": Not enough memory." << endl;
		pthread_mutex_unlock(&sharedLock);
	}
	else
	{
		for (int i = 0; i < randSize; i++)
		{
			memory[thread_message[*intID]+i] = '1';
		}
	}
}

void init()
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
	{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory
  	{char zero = '0'; memory[i] = zero;}
   	pthread_create(&server,NULL,server_function,NULL); //start server
	pthread_mutex_unlock(&sharedLock); //unlock
}

void dump_memory()
{
	// You need to print the whole memory array here.
  for (int i = 0; i < MEMORY_SIZE; i++)
	{
    cout << memory[i];
  }

}

int main (int argc, char *argv[])
 {
   pthread_t threadArray[NUM_THREADS];
   int idArray[NUM_THREADS];

   for (int i = 0; i < NUM_THREADS; i++)
	 {
     pthread_t ceren;
     threadArray[i] = ceren;
     idArray[i] = i;
   }
 	//You need to create a thread ID array here

 	init();	// call init
 	//You need to create threads with using thread ID array, using pthread_create()
  for(int i = 0; i < NUM_THREADS; i++)
	{
    pthread_create(&threadArray[i], NULL, thread_function ,(void *)&idArray[i]);
  }
 	//You need to join the threads
  for(int i = 0; i < NUM_THREADS; i++)
	{
    pthread_join(threadArray[i], NULL);
  }

 	dump_memory(); // this will print out the memory
	check = false;
 	cout << "\nMemory Indexes:\n";
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		cout << "[" << thread_message[i] << "]"; // this will print out the memory indexes
 	}
 	cout << "\nTerminating...\n";
 }
