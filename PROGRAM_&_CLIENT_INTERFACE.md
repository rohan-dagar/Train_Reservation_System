# From Program Perspective : -

## Include Libraries: 
The code includes necessary C++ libraries like <bits/stdc++.h>, <pthread.h>, <unistd.h>, and <signal.h>.

## Define Constants: 
Several constants are defined, including the number of trains, seats per train, number of threads, maximum queries, and simulation time.

## Declare Mutexes and Condition Variables: 
Mutexes (tableMutex, exitMutex) and condition variables (readPossible, writePossible) are declared to control access to shared resources and synchronization between threads.

## Define Data Structures: 
A structure tabEntry is defined to represent entries in a shared table containing information about queries. An array sharedTable holds these entries.

## Define Global Variables: 
Global variables are defined to keep track of train seats, active queries, and an exit flag for the simulation.

## Define Utility Functions: 
Utility functions like ns_to_timespec and get_current_time are defined to work with time-related operations.

## Define I/O Function: 
print_output function is defined to print output messages while ensuring thread safety using a mutex.

## Define Helper Functions: 
Functions like min, get_read_entry, get_write_entry, and get_free_entry are defined to assist in managing shared table entries.

## Define Entry Management Functions: 
Functions like add_read_entry, remove_read_entry, add_write_entry, and remove_write_entry are defined to manage read and write entries in the shared table.

## Define Query Functions: 
Functions like enquire, book, and cancel are defined to perform corresponding actions on trains, such as checking available seats, booking seats, and canceling bookings.

## Define Worker Function: 
The worker function simulates the behavior of multiple threads. Each thread randomly selects a query type (enquire, book, or cancel) and performs the corresponding action on a randomly selected train.

## Initialize Function: 
The init function initializes shared resources like mutexes, condition variables, and shared table entries.

## Cleanup Function: 
The cleanup function releases allocated resources and destroys mutexes and condition variables.

## Signal Handler Function: 
The timer_handler function is a signal handler for the alarm signal (SIGALRM). It sets the exitNow flag to true when the simulation time elapses.

## Main Function:
Seeds the random number generator and retrieves the start time for the simulation.
Initializes shared resources.
Creates multiple worker threads, each simulating a user performing queries on train seats.
Sets a timer for the simulation time using SIGALRM.
Waits for all worker threads to complete.
Cleans up allocated resources.

### This code essentially simulates multiple users (threads) interacting with a system to enquire about available seats, book seats, and cancel bookings for multiple trains. Synchronization mechanisms like mutexes and condition variables ensure thread safety and prevent race conditions.


# User Perspective : -

## Client Initialization:
The client starts by including necessary headers and libraries in their code to interact with the system.

## Query Generation:
The client generates queries to perform actions like enquiring about available seats, booking seats, or canceling bookings for trains.

## Query Execution:
The client sends the generated queries to the system for execution.

## Enquire Query:
The client sends an enquire query to the system, specifying the train for which they want to check available seats.
The system responds with the number of available seats for the specified train.

## Book Query:
The client sends a book query to the system, specifying the train and the number of seats they want to book.
The system processes the booking request, reserves the requested number of seats if available, and responds with the number of seats successfully booked.

## Cancel Query:
The client sends a cancel query to the system, specifying the train and the number of seats they want to cancel.
The system processes the cancellation request, releases the specified number of seats, and responds with a confirmation of the cancellation.

## Client Feedback:
Throughout the interaction, the client receives feedback messages from the system indicating the status of their queries and actions.
Feedback messages include information like the success or failure of queries, the number of available seats, booked seats, canceled seats, and any suspensions or resumptions in the system due to concurrency control mechanisms.

## Simulation Termination:
The client continues sending queries until they decide to end the interaction.
The simulation has a predefined time limit, after which the system terminates automatically.
The client receives a termination notification once the simulation time elapses.


### This step-by-step guide outlines how a client can interact with the system implemented in the code, including the types of queries they can send and the responses they can expect to receive.
