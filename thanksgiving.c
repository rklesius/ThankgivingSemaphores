//This program helps people decide on where to sit at the thanksgiving
//dinner table based on what food they want and the least amount of passing
//of food required to get them the food they want using semaphores.      
//By Rachel Klesius and Garrett Moncrief

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

// key number
#define SHMKEY1 ((key_t) 1497)
#define SHMKEY2 ((key_t) 1498)
#define SHMKEY3 ((key_t) 1499)
#define SHMKEY4 ((key_t) 1500)
#define SHMKEY5 ((key_t) 1501)
#define SHMKEY6 ((key_t) 1502)
#define PEOPLE_COUNT 4

typedef struct 
{
  int value;   // Count of how many passes
  int curPos;  // Current location of process
} shared_mem;

// Food shared resources, values of these will indicate how many times they have
// been passsed
shared_mem *turkey;
shared_mem *gravy;
shared_mem *mash;
shared_mem *stuffing;
shared_mem *peas;
shared_mem *cranberry;

// For Semaphores
#define SEMKEY ((key_t) 400L)  //semaphore key
#define NSEMS 6 //number of semaphores being created
int sem_id;
static struct sembuf OP = {0, -1, 0};
static struct sembuf OV = {0, 1, 0};
struct sembuf *P = &OP;
struct sembuf *V = &OV;

// Semaphore union used to generate semaphore
typedef union {
	int val;
	struct semid_ds *buf;
	ushort *array;
} semunion;  

// POP (wait()) function for semaphore to protect critical section
int POP()
{
	int status;
	status = semop(sem_id, P, 1);
	return status;
}

// VOP (signal()) function for semaphore to release protection 
int VOP()
{
	int status;
	status = semop(sem_id, V, 1);
	return status;
}

//People are processes, all trying to get their plate full
void rachel(int position);
void garrett(int position);
void eddie(int position);
void megan(int position);

int main()
{
	int shmid1, shmid2, shmid3, shmid4, shmid5, shmid6, pid1, pid2, pid3, pid4, ID, status;
	char *shmadd;
  shmadd = (char *) 0;
 
  // For order of people at table
 
	// For semaphores
	int value, value1;
	semunion semctl_arg;
	semctl_arg.val = 1;

	// Create and connect to a shared memory segment
	if ((shmid1 = shmget(SHMKEY1, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
  if ((turkey = (shared_mem *) shmat(shmid1, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}
 
 
	if ((shmid2 = shmget(SHMKEY2, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
	if ((gravy = (shared_mem *) shmat(shmid2, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}
 
 
 	if ((shmid3 = shmget(SHMKEY3, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
  if ((mash = (shared_mem *) shmat(shmid3, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}
 
 
	if ((shmid4 = shmget(SHMKEY4, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
  if ((stuffing = (shared_mem *) shmat(shmid4, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}
  
  
 	if ((shmid5 = shmget(SHMKEY5, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
  if ((peas = (shared_mem *) shmat(shmid5, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}
 
 
	if ((shmid6 = shmget(SHMKEY6, sizeof(int), IPC_CREAT | 0666)) < 0)
	{
		perror("shmget");
		exit(1);
	}
  if ((cranberry = (shared_mem *) shmat(shmid6, shmadd, 0)) == (shared_mem *)-1)
	{
		perror("shmat");
		exit(0);
	}

  //initialize the pass count to zero and location to -1, showing that no one has picked it up yet
	turkey->value = 0;
  turkey->curPos = -1; 
  gravy->value = 0;
  gravy->curPos = -1;
  mash->value = 0;
  mash->curPos = -1;
  stuffing->value = 0;
  stuffing->curPos = -1;
  peas->value = 0;
  peas->curPos = -1;
  cranberry->value = 0;
  cranberry->curPos = -1;
	
	// Create Semaphores
	sem_id = semget(SEMKEY, NSEMS, IPC_CREAT | 0666);
	if (sem_id < 0)
	{
		printf("Error in creating the semaphore. \n");
	}
	
	// Initialize Semaphores
	value1 = semctl(sem_id, 0, SETVAL, semctl_arg);
	value = semctl(sem_id, 0, GETVAL, semctl_arg);
	if (value < 0)
	{
		printf("Error detected in SETVAL.  \n");
	}
	
	//Fork children in the order of the table based on their seat
 
	if ((pid1 = fork()) == 0)
	{
		rachel(1);
	}
	if ((pid3 = fork()) == 0)
	{
		eddie(2);
	}
	if ((pid4 = fork()) == 0)
	{
		megan(3);
	}
	if ((pid2 = fork()) == 0)
	{
		garrett(4);
	}

	// Wait for child processes and display id
	int ID1 = wait(&pid1);
	int ID2 = wait(&pid2);
	int ID3 = wait(&pid3);
	int ID4 = wait(&pid4);
	printf("Child with ID: %d has just exited\n", ID1);
	printf("Child with ID: %d has just exited\n", ID2);
	printf("Child with ID: %d has just exited\n", ID3);
	printf("Child with ID: %d has just exited\n", ID4);
 
  //Print times each food passed
  printf("Turkey passed: %d times\n", turkey->value);
  printf("Gravy passed: %d times\n", gravy->value);
  printf("Mashed potatoes passed: %d times\n", mash->value);
  printf("Stuffing passed: %d times\n", stuffing->value);
  printf("Peas passed: %d times\n", peas->value);
  printf("Cranberry sauce passed: %d times\n", cranberry->value);

	/* ends shared memory */
	shmdt(turkey);
  shmdt(gravy);
  shmdt(mash);
  shmdt(stuffing);
  shmdt(peas);
  shmdt(cranberry);
	shmctl(shmid1, IPC_RMID, NULL);
  shmctl(shmid2, IPC_RMID, NULL);
  shmctl(shmid3, IPC_RMID, NULL);
  shmctl(shmid4, IPC_RMID, NULL);
  shmctl(shmid5, IPC_RMID, NULL);
  shmctl(shmid6, IPC_RMID, NULL);

	/* Deallocate semaphore */
	semctl_arg.val = 0;
	status = semctl(sem_id, 0, IPC_RMID, semctl_arg);
	if(status < 0)
	{
		printf("Error in removing the semaphore. \n");
	}

	printf("\nEnd of Simulation\n");
	return 0;
} 

//Rachel wants everything but stuffing from Thanksgiving
void rachel(int position)
{	
  // Calculate neighbors for this position
  int leftNeighbor;
  int rightNeighbor;
  
  if(position == 0)  //first in row
  {
    leftNeighbor = PEOPLE_COUNT - 1;
    rightNeighbor = 1;
  
  else if(position == PEOPLE_COUNT - 1)  //last in row
  {
    leftNeighbor = PEOPLE_COUNT - 2;
    rightNeighbor = 0;
  }
  
  else 
  {
    leftNeighbor = position - 1;
    rightNeighbor = position + 1;
  }
  
  while(true) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (turkey->curPos == -1 || turkey->curPos == leftNeighbor || turkey->curPos == rightNeighbor)  //TODO: change to check for neighbors
    {
      turkey->value++;
      break;
    }
    turkey->curPos = position;
    VOP();
  }
  printf("\nTurkey passed: %d times\n", turkey->value);
  shmdt(turkey);	
 
 /*
  POP(); 
  gravy->value++;
  VOP();
  printf("Gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  POP(); 
  mash->value++;
  VOP();
  printf("Mashed Potatoes passed: %d times\n", mash->value);
  shmdt(mash);
  
  POP(); 
  peas->value++;
  VOP();
  printf("Peas passed: %d times\n", peas->value);
  shmdt(peas);
  
  POP(); 
  cranberry->value++;
  VOP();
  printf("Cranberry sauce passed: %d times\n", cranberry->value);
  shmdt(cranberry);
  */
 
	_exit(0);
}

//Garrett wants everything from thanksgiving
void garrett()
{
	POP(); 
  turkey->value++;
  VOP();
  printf("\nTurkey passed: %d times\n", turkey->value);
  shmdt(turkey);
  
  /*
  POP(); 
  gravy->value++;
  VOP();
  printf("Gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  POP(); 
  mash->value++;
  VOP();
  printf("Mashed potatoes passed: %d times\n", mash->value);
  shmdt(mash);
  
  POP();
  stuffing->value++;
  VOP();
  printf("Stuffing passed: %d times\n", stuffing->value);
  shmdt(stuffing);
  
  POP(); 
  peas->value++;
  VOP();
  printf("Peas passed: %d times\n", peas->value);
  shmdt(peas);
  
  POP(); 
  cranberry->value++;
  VOP();
  printf("\nCranberry sauce passed: %d times\n", cranberry->value);
  shmdt(cranberry);
  */
 
	_exit(0);
}

//eddie wants turkey, gravy, and mashed potatoes
void eddie()
{	
	POP(); 
  turkey->value++;
  VOP();
  printf("\nTurkey passed: %d times\n", turkey->value);
  shmdt(turkey);
	/*
  POP(); 
  gravy->value++;
  VOP();
  printf("Gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  POP(); 
  mash->value++;
  VOP();
  printf("\nTurkey passed: %d times\n", turkey->value);	
  shmdt(mash);
 */
	_exit(0);
}

//megan wants turkey, gravy, stuffing, and peas
void megan()
{	
  POP(); 
  turkey->value++;
  VOP();
  printf("\nTurkey passed: %d times\n", turkey->value);
  shmdt(turkey);
	/*
  POP(); 
  gravy->value++;
  VOP();
  printf("Gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  POP();
  stuffing->value++;
  VOP();
  printf("Stuffing passed %d times\n", stuffing->value);
  shmdt(stuffing);
  
  POP(); 
  peas->value++;
  VOP();
  printf("Peas passed %d times\n", peas->value);
  shmdt(peas);
  */
 
	_exit(0);
}
 
