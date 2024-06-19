# Train Reservation System

## Introduction

This project is a multi-threaded ticket-reservation system for a single day, developed in C++. The system manages ticket reservations for multiple trains, ensuring consistency and efficiency through mutual exclusion and query limits.

## Features

- **Inquire Seats**: Check the number of available seats in a train.
- **Book Tickets**: Reserve a specified number of tickets in a train.
- **Cancel Tickets**: Cancel a previously booked ticket.
- **Concurrency**: Supports multiple concurrent queries with a limit to avoid overloading.
- **Mutual Exclusion**: Ensures database consistency with proper handling of read/write operations.
- **Automatic Query Generation**: Randomly generates query types, train numbers, and booking/cancellation requests.
- **Diagnostic Messages**: Prints detailed logs of query inputs, outputs, thread activities, and synchronization events.

## Parameters

- `t`: Number of trains (default: 100)
- `c`: Capacity of each train (default: 500)
- `k`: Number of tickets booked in a query (random integer between 5 and 10)
- `s`: Number of worker threads (default: 20)
- `MAX`: Maximum number of concurrent active queries (default: 5)
- `T`: Total running time (1 to 10 minutes)

## Implementation Details

- **Main Thread**: Initializes trains and creates worker threads.
- **Worker Threads**: Generate and execute queries, ensuring proper synchronization and mutual exclusion.
- **Shared Table**: Manages active queries, ensuring no conflicts in read/write operations.
- **Synchronization**: Uses condition variables and mutexes to manage query limits and ensure database consistency.
- **Termination**: Worker threads run for a predetermined time `T`, then exit. The main thread prints the reservation status before exiting.

## Usage

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/yourusername/train-reservation-system.git
   cd train-reservation-system
   ```

2. **Compile the Code**:
   ```sh
   g++ -pthread -o train_reservation train_reservation.cpp
   ```

3. **Run the Program**:
   ```sh
   ./train_reservation
   ```

## Output

- **Diagnostic Messages**: Displays query details, thread numbers, waiting and signaling status, and more.
- **Final Reservation Status**: Prints the current reservation status of all trains after the simulation.

## Dependencies

- C++11 or later
- pthread library
