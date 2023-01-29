#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>//To generate a random number
#include <pthread.h>
#include <semaphore.h>


//Define global static variables.
#define CUSTOMER_NUMBER 25
#define CUSTOMER_ARRIVAL_TIME_MIN 1
#define CUSTOMER_ARRIVAL_TIME_MAX 3
#define REGISTER_NUMBER 5
#define COFFEE_TIME_MIN 2
#define COFFEE_TIME_MAX 5

// Define the semaphores.
sem_t registerS;;//Semaphore for the registers.
sem_t customerS;//Semaphore for the customers.
sem_t mutex;//Semaphore for the mutex.

int *register_num;//It allows us to know which register is used by which customer.
int rand_arrival_time = 0;//Random time taken to create customer.
int rand_coffee_time = 0;//The random number it takes for a register to prepare the coffee.
int random_generator(int lower, int upper);//Define the function.

int readyCustomer = -1;//It is used to define the customer who comes ready.
int counter = -1;//We needed a variable to understand which customer went to which register.

// Register thread function:
 void Register (void *id){
 	
 	int registerID = *(int*)id;//The parameter from the register function is assigned to the register ID.
 	int customerID;//For the IDs of the customers.
 	
 	while(1){//Spinning in an endless loop until the customer arrives.
 	
 		//Critical area is under control.
 		sem_wait(&registerS);//Every register is stopped until it reaches this stage.
 		sem_wait(&mutex);//From here it comes back to the beginning of the loop.
 		//After that, customers started to form and the remaining registers started to work sequentially.
 			
 		//The incoming customer is directed to the empty register. 	
 		readyCustomer = ++readyCustomer;//The ready customer value is increased so that the next customer is passed.
 		customerID = register_num[readyCustomer];//The customer's ID is assigned to the customer's ID that reaches the register.
 		register_num[readyCustomer] = pthread_self();//Returns the ID of the thread in which it is invoked.
 		
 		sem_post(&mutex);//Release lock.
 		sem_post(&customerS);//Signal to customer which mean register is ready.
 		
 		printf("\nCUSTOMER <%d%s%d%s\n", customerID,"> GOES TO REGISTER <",registerID,">.\n");	 
		sleep(rand_coffee_time);//Waiting for the coffee to be prepared.
		printf("\nCUSTOMER <%d%s%d%s%d%s\n",customerID ,"> FINISHED BUYING FROM REGISTER <",registerID,"> AFTER <" ,rand_coffee_time,"> SECONDS.\n");
	}		
	pthread_exit(0);//This function was used to terminate its own thread flow within a thread flow.
}

// customer thread function:
void Customer(void *id){
	
	int customersID = *(int*)id;//The parameter from the customer function is assigned to the customers ID.
	int registersID, temp;//For the IDs of the regisers.And initialize the temp.
	
	sem_wait(&mutex);//To access the number of empty registers.
	
	printf("\nCUSTOMER <%d%s%d%s\n", customersID , "> IS CREATED AFTER <", rand_arrival_time , "> SECONDS.\n");
	counter = ++counter;//Increasing the counter.
	temp = counter;//Assignment of value to temp.
	register_num[temp] = customersID;//Assigning the current customer to the register array.
	
	sem_post(&mutex);//Release lock.
	sem_post(&registerS);//Signal is sent for the appropriate register.
	sem_wait(&customerS);//It allows the customer to queue up.
	sem_wait(&mutex);
	
	registersID = register_num[temp];//Whichever customer the register is preparing coffee for is kept in the array.
	
	sem_post(&mutex);//Release lock.
	pthread_exit(0);//This function was used to terminate its own thread flow within a thread flow.
}


int main(int argc){// Main method of the code    
	
	int i, j,k,err;//Initialize the variables.
	register_num = (int*) malloc(sizeof(int) * CUSTOMER_NUMBER);//It means addressing as many customers as possible.
	
	pthread_t registerr[REGISTER_NUMBER];//An array of register threads with max size of 5. 
	pthread_t customer[CUSTOMER_NUMBER];//An array of customer threads with max size of 25.
	
	// Initializing semaphores:
	 // zero indicates that this semaphore is not allowed to be shared between processes, 1 is an initial value
	sem_init(&registerS, 0, 0);//for registers semophore.
	sem_init(&customerS, 0, 0);//for customers semophore.
	sem_init(&mutex, 0, 1);// the initial value is 1 because we need to allow only one thread to access
	
	printf("CAFE OPENED.\n");
	
	  // Creating registers threads:
	for(i = 0; i < REGISTER_NUMBER; i++){
	    err=pthread_create(&registerr[i], NULL, (void*)Register, (void*)&i);//A thread is created for each register.
		if (err != 0)//To check if there are any errors when creating the thread.
		   printf("Thread creation error: [%s]",strerror(err));
		sleep(1);
	}
	  // Creating customers threads:
	for(j = 0; j < CUSTOMER_NUMBER; j++){
		err=pthread_create(&customer[j], NULL, (void*)Customer, (void*)&j);//A thread is created for each customer.
		if (err != 0)//To check if there are any errors when creating the thread.
			printf("Thread creation error: [%s]",strerror(err));				
        rand_arrival_time = random_generator(CUSTOMER_ARRIVAL_TIME_MIN, CUSTOMER_ARRIVAL_TIME_MAX);//Generating random time.	
		rand_coffee_time = random_generator(COFFEE_TIME_MIN, COFFEE_TIME_MAX);//Generating random time. 	
      	sleep(rand_arrival_time);//Waiting for the create customer.
	}
	
	for(k = 0; k < CUSTOMER_NUMBER; k++){
		pthread_join(customer[k], NULL);//Waits until the fiber with the given parameter runs out (join).
	}
	
	//Destroys the semaphore object, freeing any resources it may have.
	sem_destroy(&registerS);//For the registers.
	sem_destroy(&customerS);//For the customers.
	sem_destroy(&mutex);//For the mutex.
	
	printf("\nCAFE CLOSED.");
	return 0;
}

//Generates a random number among the received parameters.
int random_generator(int lower, int upper){	
	int num = (rand() % (upper - lower + 1)) + lower;//Random number generating part.
	return num;//Causes the function to return the found value.
}






