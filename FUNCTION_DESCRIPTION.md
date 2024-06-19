# Explanation of Pthread Functions:
Pthread Functions Used:

## pthread_mutex_init(&mutex, NULL):
Initializes a mutex with the given attributes (or NULL for default attributes).
This function prepares the mutex for use in locking and unlocking operations.

## pthread_mutex_lock(&mutex):
Locks the specified mutex.
If the mutex is already locked by another thread, the calling thread will wait until the mutex becomes available.

## pthread_mutex_unlock(&mutex):
Unlocks the specified mutex.
If there are threads waiting for this mutex, one of them will be unblocked and allowed to proceed.

## pthread_mutex_destroy(&mutex):
Destroys the specified mutex.
Releases any resources associated with the mutex.

## pthread_cond_init(&cond, NULL):
Initializes a condition variable with the given attributes (or NULL for default attributes).
This function prepares the condition variable for use in signaling and waiting operations.

## pthread_cond_wait(&cond, &mutex):
Atomically releases the mutex and waits for the condition variable to be signaled.
Upon return, the mutex is locked again before the function returns.

## pthread_cond_broadcast(&cond):
Signals all threads waiting on the specified condition variable.
It wakes up all threads that are waiting for the condition variable to be signaled.

## pthread_cond_destroy(&cond):
Destroys the specified condition variable.
Releases any resources associated with the condition variable.

## pthread_create(&thread, NULL, start_routine, arg):
Creates a new thread and starts execution of the function start_routine with the given arg.
The new thread starts executing in the start_routine, passing arg as the argument.

## pthread_join(thread, &retval):
Waits for the specified thread to terminate.
Returns when the specified thread terminates, optionally storing the thread's exit status in retval.

## pthread_exit(retval):
Terminates the calling thread and returns a value.
This function is typically called when the thread has finished its work and needs to exit.


# Explanation of Code Functions:

## Data Structures Used:

## tabEntry:
Represents an entry in the shared table.
Contains information about a train ID, query type, and number of threads.

## sharedTable[MAX_QUERIES]:
An array of tabEntry pointers representing the shared table.
Stores information about active queries.

## trainSeats[TRAINS]:
An array representing the available seats for each train.


## Functions:

## getNewEntry(int trainID, int qtype, int n):
Creates a new tabEntry with the given parameters.

## init():
Initializes global variables, mutexes, condition variables, and shared table entries.

## cleanup():
Cleans up resources such as mutexes, condition variables, and shared table entries.

## timer_handler(int signum):
Signal handler function called when the alarm signal is received.
Sets the exitNow flag to true, indicating that threads should exit.

## worker(void arg):*
Function executed by worker threads.
Performs queries (enquire, book, cancel) randomly on trains.

## add_read_entry(int trainID):
Adds a read entry to the shared table for the specified train.
Handles suspension if the maximum query limit is reached or a write entry exists.

## remove_read_entry(int trainID):
Removes a read entry from the shared table for the specified train.
Signals threads waiting to write if no more read entries exist for the train.

## enquire(int trainID):
Performs an enquire query on the specified train.
Retrieves the available seats for the train.

## add_write_entry(int trainID):
Adds a write entry to the shared table for the specified train.
Handles suspension if the maximum query limit is reached or read/write entries exist.

## remove_write_entry(int trainID):
Removes a write entry from the shared table for the specified train.
Signals threads waiting to read/write if no more write entries exist for the train.

## book(int trainID, int k):
Performs a booking operation for the specified number of seats on the train.
Updates the available seats and returns the number of seats booked.

## cancel(int trainID, int k):
Performs a cancellation operation for the specified number of seats on the train.
Updates the available seats.

## main():
Initializes resources, creates worker threads, sets up the alarm signal, and waits for threads to finish execution.




