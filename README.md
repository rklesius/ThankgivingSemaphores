# Thankgiving Semaphores

For our final project for Operating Systems, we explored the concept of semaphores in a novel way: by representing the behavior of semaphore protected shared memory segments with a Thanksgiving dinner. In this program there are a set number of people who are invited to dinner (the programs) and a set number of different dishes (the shared memory segments). The program will find the number of passes each dish experiences based on which guest requests which dish.  This shows multiple semaphores, processes that need a variable amount of those semaphores, and having to execute those processes in a specific order.  

To run this program, simply use a gcc compiler with the commands:<br>
gcc thanksgiving.c<br>
./a.out
