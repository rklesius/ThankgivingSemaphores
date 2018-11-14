//This program helps people decide on where to sit at the thanksgiving
//dinner table based on what food they want and the least amount of passing
//of food required to get them the food they want using semaphores.      
//By Rachel Klesius and Garrett Moncrief

#include <stdio.h>
#include <stdbool.h>
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
  int need;    // How many people need this dish
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

  //initialize the pass count to zero and location to -1, showing no one has picked it up
  //Also, set the need of each resources based on what people want
	turkey->value = 0;
  turkey->curPos = -1; 
  turkey->need = 4;
  
  gravy->value = 0;
  gravy->curPos = -1;
  gravy->need = 4;
  
  mash->value = 0;
  mash->curPos = -1;
  mash->need = 3;
  
  stuffing->value = 0;
  stuffing->curPos = -1;
  stuffing->need = 2;
  
  peas->value = 0;
  peas->curPos = -1;
  peas->need = 3;
  
  cranberry->value = 0;
  cranberry->curPos = -1;
  cranberry->need = 2;
	
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
		rachel(3);
	}
	if ((pid3 = fork()) == 0)
	{
		eddie(2);
	}
	if ((pid4 = fork()) == 0)
	{
		megan(1);
	}
	if ((pid2 = fork()) == 0)
	{
		garrett(0);
	}

	// Wait for child processes and display id
  int ID4 = wait(&pid4);
	int ID1 = wait(&pid1);
	int ID2 = wait(&pid2);
	int ID3 = wait(&pid3);
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
  int neighbor;
  
  //To make sure Rachel gets her grub
  bool gotTurkey = false;
  bool gotGravy = false;
  bool gotMash = false;
  bool gotPeas = false;
  bool gotCranberry = false;
  
  //what Rachel will pass if she needs to
  bool passedStuffing = false;
  
  if(position == 0)  //first in row
  {
    neighbor = PEOPLE_COUNT - 1;
  }
  else if(position == PEOPLE_COUNT - 1)  //last in row
  {
    neighbor = PEOPLE_COUNT - 2;
  }
  
  else 
  {
    neighbor = position - 1;
  }
  
  while(!gotTurkey) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor has it, take it
    if (turkey->curPos == -1 || turkey->curPos == neighbor)
    {
      turkey->value++;
      turkey->need--;
      turkey->curPos = position;
      gotTurkey = true;
    }
    VOP();
  }
  printf("\nRachel got turkey, turkey passed: %d times\n", turkey->value);
  shmdt(turkey);
  
  while(!gotGravy) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (gravy->curPos == -1 || gravy->curPos == neighbor)
    {
      gravy->value++;
      gravy->need--;
      gravy->curPos = position;
      gotGravy = true;
    }
    VOP();
  }
  printf("Rachel got gravy, gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  while(!gotMash) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (mash->curPos == -1 || mash->curPos == neighbor)
    {
      mash->value++;
      mash->need--;
      mash->curPos = position;
      gotMash = true;
    }
    VOP();
  }
  printf("Rachel got mashed potatoes, mashed potatoes passed: %d times\n", mash->value);
  shmdt(mash);	
  
  //Rachel doesn't want stuffing, but she still has to pass it if she knows other people need it and it comes to her
  while(!passedStuffing)
  {
    POP();
    //if her neighbor gets stuffing and there is still a need for stuffing she must pass it on
    if((stuffing->need > 0) && (stuffing->curPos == neighbor))
    {
      stuffing->value++;
      stuffing->curPos = position;
      passedStuffing = true;
    }
    
    //if there is no more need for stuffing, exit this loop without adding to passing count
    if(stuffing->need == 0)
    {
      passedStuffing = true;
    }
    VOP();
  }
  shmdt(stuffing);
 
  while(!gotPeas) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (peas->curPos == -1 || peas->curPos == neighbor)
    {
      peas->value++;
      peas->need--;
      peas->curPos = position;
      gotPeas = true;
    }
    VOP();
  }
  printf("Rachel got peas, peas passed: %d times\n", peas->value);
  shmdt(peas);
  
  while(!gotCranberry) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (cranberry->curPos == -1 || cranberry->curPos == neighbor)
    {
      cranberry->value++;
      cranberry->need--;
      cranberry->curPos = position;
      gotCranberry = true;
    }
    VOP();
  }
  printf("Rachel got cranberry sauce, cranberry sauce passed: %d times\n", cranberry->value);
  shmdt(cranberry);
 
	_exit(0);
}

//Garrett wants everything from thanksgiving
void garrett(int position)
{
  // Calculate neighbor for this position
  int neighbor;

  bool gotTurkey = false;
  bool gotGravy = false;
  bool gotMash = false;
  bool gotStuffing = false;
  bool gotPeas = false;
  bool gotCranberry = false;
  
  if(position == 0)  //first in row
  {
    neighbor = PEOPLE_COUNT - 1;
  }
  else if(position == PEOPLE_COUNT - 1)  //last in row
  {
    neighbor = PEOPLE_COUNT - 2;
  }
  
  else 
  {
    neighbor = position - 1;
  }
  
  while(!gotTurkey) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor has it, take it
    if (turkey->curPos == -1 || turkey->curPos == neighbor)
    {
      turkey->value++;
      turkey->need--;
      turkey->curPos = position;
      gotTurkey = true;
    }	
   VOP();
  }
  printf("Garrett got turkey, turkey passed: %d times\n", turkey->value);
  shmdt(turkey);
  
  while(!gotGravy) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (gravy->curPos == -1 || gravy->curPos == neighbor)
    {
      gravy->value++;
      gravy->need--;
      gravy->curPos = position;
      gotGravy = true;
    }
    VOP();
  }
  printf("Garrett got gravy, gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  while(!gotMash) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (mash->curPos == -1 || mash->curPos == neighbor)
    {
      mash->value++;
      mash->need--;
      mash->curPos = position;
      gotMash = true;
    }
    VOP();
  }
  printf("Garrett got mashed potatoes, mashed potatoes passed: %d times\n", mash->value);
  shmdt(mash);
  
  while(!gotStuffing) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (stuffing->curPos == -1 || stuffing->curPos == neighbor)
    {
      stuffing->value++;
      stuffing->need--;
      stuffing->curPos = position;
      gotStuffing = true;
    }
    VOP();
  }
  printf("Garrett got stuffing, stuffing passed: %d times\n", stuffing->value);
  shmdt(stuffing);
  
  while(!gotPeas) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (peas->curPos == -1 || peas->curPos == neighbor)
    {
      peas->value++;
      peas->need--;
      peas->curPos = position;
      gotPeas = true;
    }
    VOP();
  }
  printf("Garrett got peas, peas passed: %d times\n", peas->value);
  shmdt(peas);
  
  while(!gotCranberry) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (cranberry->curPos == -1 || cranberry->curPos == neighbor)
    {
      cranberry->value++;
      cranberry->need--;
      cranberry->curPos = position;
      gotCranberry = true;
    }
    VOP();
  }
  printf("Garrett got cranberry sauce, cranberry sauce passed: %d times\n", cranberry->value);
  shmdt(cranberry);
   
	_exit(0);
}

//eddie wants turkey, gravy, and mashed potatoes
void eddie(int position)
{	
	  // Calculate neighbors for this position
  int neighbor;
  bool gotTurkey = false;
  bool gotGravy = false;
  bool gotMash = false;
  
  bool passedStuffing = false;
  bool passedPeas = false;
  bool passedCranberry = false;
  
  if(position == 0)  //first in row
  {
    neighbor = PEOPLE_COUNT - 1;
  }
  else if(position == PEOPLE_COUNT - 1)  //last in row
  {
    neighbor = PEOPLE_COUNT - 2;
  }
  
  else 
  {
    neighbor = position - 1;
  }
  
  while(!gotTurkey) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor has it, take it
    if (turkey->curPos == -1 || turkey->curPos == neighbor)
    {
      turkey->value++;
      turkey->need--;
      turkey->curPos = position;
      gotTurkey = true;
    }
    VOP();
  }
  printf("\nEddie got turkey, turkey passed: %d times\n", turkey->value);
  shmdt(turkey);	
  
  while(!gotGravy) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (gravy->curPos == -1 || gravy->curPos == neighbor)
    {
      gravy->value++;
      gravy->need--;
      gravy->curPos = position;
      gotGravy = true;
    }
    VOP();
  }
  printf("Eddie got gravy, gravy passed: %d times\n", gravy->value);
  shmdt(gravy);
  
  while(!gotMash) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (mash->curPos == -1 || mash->curPos == neighbor)
    {
      mash->value++;
      mash->need--;
      mash->curPos = position;
      gotMash = true;
    }
    VOP();
  }
  printf("Eddie got mashed potatoes, mashed potatoes passed: %d times\n", mash->value);
  shmdt(mash);
  
  while(!passedStuffing)
  {
    POP();
    
    if((stuffing->need > 0) && (stuffing->curPos == neighbor))
    {
      stuffing->value++;
      stuffing->curPos = position;
      passedStuffing = true;
    }
    
    if(stuffing->need == 0)
    {
      passedStuffing = true;
    }
    VOP();
  }
  shmdt(stuffing);
  
  while(!passedPeas)
  {
    POP();
    
    if((peas->need > 0) && (peas->curPos == neighbor))
    {
      peas->value++;
      peas->curPos = position;
      passedPeas = true;
    }
    
    if(peas->need == 0)
    {
      passedPeas = true;
    }
    VOP();
  }
  shmdt(peas);
  
  while(!passedCranberry)
  {
    POP();
    
    if((cranberry->need > 0) && (cranberry->curPos == neighbor))
    {
      cranberry->value++;
      cranberry->curPos = position;
      passedCranberry = true;
    }
    
    if(cranberry->need == 0)
    {
      passedCranberry = true;
    }
    VOP();
  }
  shmdt(cranberry);
  
	_exit(0);
}

//megan wants turkey, gravy, stuffing, and peas
void megan(int position)
{	
  // Calculate neighbors for this position
  int neighbor;
  
  bool gotTurkey = false;
  bool gotGravy = false;
  bool gotStuffing = false;
  bool gotPeas = false;
  
  bool passedMash = false;
  bool passedCranberry = false;
  
  if(position == 0)  //first in row
  {
    neighbor = PEOPLE_COUNT - 1;
  }
  else if(position == PEOPLE_COUNT - 1)  //last in row
  {
    neighbor = PEOPLE_COUNT - 2;
  }
  
  else 
  {
    neighbor = position - 1;
  }
  
  while(!gotTurkey) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor has it, take it
    if (turkey->curPos == -1 || turkey->curPos == neighbor)
    {
      turkey->value++;
      turkey->need--;
      turkey->curPos = position;
      gotTurkey = true;
    }
    VOP();
  }
  printf("\nMegan got turkey, turkey passed: %d times\n", turkey->value);
  shmdt(turkey);
  
  while(!gotGravy) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (gravy->curPos == -1 || gravy->curPos == neighbor)
    {
      gravy->value++;
      gravy->need--;
      gravy->curPos = position;
      gotGravy = true;
    }
    VOP();
  }
  printf("Megan got gravy, gravy passed: %d times\n", gravy->value);
  shmdt(gravy);	
  
  while(!passedMash)
  {
    POP();
    
    if((mash->need > 0) && (mash->curPos == neighbor))
    {
      mash->value++;
      mash->curPos = position;
      passedMash = true;
    }
    
    if(mash->need == 0)
    {
      passedMash = true;
    }
    VOP();
  }
  shmdt(mash);
  
  while(!gotStuffing) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (stuffing->curPos == -1 || stuffing->curPos == neighbor)
    {
      stuffing->value++;
      stuffing->need--;
      stuffing->curPos = position;
      gotStuffing = true;
    }
    VOP();
  }
  printf("Megan got stuffing, stuffing passed: %d times\n", stuffing->value);
  shmdt(stuffing);
  
  while(!gotPeas) 
  {
    POP(); 
    //if no one has picked it up yet or your neighbor last used the resource, then you get the resource
    if (peas->curPos == -1 || peas->curPos == neighbor)
    {
      peas->value++;
      peas->need--;
      peas->curPos = position;
      gotPeas = true;
    }
    VOP();
  }
  printf("Megan got peas, peas passed: %d times\n", peas->value);
  shmdt(peas);
  
  while(!passedCranberry)
  {
    POP();
    
    if((cranberry->need > 0) && (cranberry->curPos == neighbor))
    {
      cranberry->value++;
      cranberry->curPos = position;
      passedCranberry = true;
    }
    
    if(cranberry->need == 0)
    {
      passedCranberry = true;
    }
    VOP();
  }
  shmdt(cranberry);
 
	_exit(0);
}
 
