#include <bits/stdc++.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

#define TRAINS 100
#define SEATS_PER_TRAIN 500
#define NUM_THREADS 20
#define MAX_QUERIES 5
#define SIMULATION_TIME 1

pthread_mutex_t consoleMutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct tabEntry {
    int trainID;
    int queryType;
    int numOfThreads;
} tabEntry;

tabEntry* getNewEntry(int trainID, int qtype, int n) {
    tabEntry* t = (tabEntry*) malloc(sizeof(tabEntry));
    t->trainID = trainID;
    t->queryType = qtype;
    t->numOfThreads = n;
    return t;
}

tabEntry* sharedTable[MAX_QUERIES];

int trainSeats[TRAINS];
int activeQueries;

pthread_mutex_t tableMutex;
pthread_mutex_t exitMutex;
pthread_cond_t readPossible[TRAINS];
pthread_cond_t writePossible[TRAINS];
volatile bool exitNow;
struct timespec start_time;

struct timespec ns_to_timespec(long long ns) {
    struct timespec ts;
    ts.tv_sec = ns / 1000000000;
    ts.tv_nsec = ns % 1000000000;
    return ts;
}

struct timespec get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    long long elapsed_ns = (ts.tv_sec - start_time.tv_sec) * 1000000000LL + (ts.tv_nsec - start_time.tv_nsec);
    return ns_to_timespec(elapsed_ns);
}

void print_output(const string& message) {
    pthread_mutex_lock(&consoleMutex);
    cout << message << flush;
    pthread_mutex_unlock(&consoleMutex);
}

int min(int a, int b) {
    return (a < b) ? a : b;
}

tabEntry* get_read_entry(int trainID) {
    for (int i = 0; i < MAX_QUERIES; i++) {
        tabEntry* query = sharedTable[i];
        if (query->trainID == trainID && query->queryType == 0)
            return query;
    }
    return NULL;
}

tabEntry* get_write_entry(int trainID) {
    for (int i = 0; i < MAX_QUERIES; i++) {
        tabEntry* query = sharedTable[i];
        if (query->trainID == trainID && query->queryType == 1)
            return query;
    }
    return NULL;
}

tabEntry* get_free_entry() {
    for (int i = 0; i < MAX_QUERIES; i++) {
        tabEntry* query = sharedTable[i];
        if (query->trainID == -1)
            return query;
    }
    return NULL;
}

void signal_free_cond() {
    for (int i = 0; i < TRAINS; i++) {
        pthread_cond_broadcast(&readPossible[i]);
        pthread_cond_broadcast(&writePossible[i]);
    }
}

void add_read_entry(int trainID) {
    pthread_mutex_lock(&tableMutex);
    struct timespec ts;
    tabEntry* p;
    int a = 0;
    while ((a = activeQueries) >= MAX_QUERIES || (p = get_write_entry(trainID))) {
        ts = get_current_time();
        if (a >= MAX_QUERIES) {
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : SUSPENDED - MAX_QUERIES reached\n\n");
        }
        else {
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : SUSPENDED - write entry for train " + to_string(trainID) + " found\n\n");
        }
        pthread_cond_wait(&readPossible[trainID], &tableMutex);
        ts = get_current_time();
        print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : RESUMING - signal received\n\n");
    }

    activeQueries++;

    p = get_read_entry(trainID);
    if (!p) {
        tabEntry* freeEntry = get_free_entry();
        freeEntry->trainID = trainID;
        freeEntry->numOfThreads = 1;
        freeEntry->queryType = 0;
    }
    else {
        p->numOfThreads += 1;
    }
    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : read entry made\n\n");

    pthread_mutex_unlock(&tableMutex);
}

void remove_read_entry(int trainID) {
    pthread_mutex_lock(&tableMutex);
    tabEntry* p = get_read_entry(trainID);
    struct timespec ts;
    p->numOfThreads -= 1;
    if (p->numOfThreads == 0) {
        p->trainID = -1;
        ts = get_current_time();
        print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Last read entry removed. waking up all the threads waiting to write train " + to_string(trainID) + "\n\n");
        pthread_cond_broadcast(&writePossible[trainID]);
    }
    else {
        ts = get_current_time();
        print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : read entry removed\n\n");
    }

    activeQueries -= 1;

    signal_free_cond();
    pthread_mutex_unlock(&tableMutex);
}

int enquire(int trainID) {
    struct timespec ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to make a read entry in shared table\n\n");

    add_read_entry(trainID);

    int availableSeats = trainSeats[trainID];

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : available seats for the train " + to_string(trainID) + " = " + to_string(availableSeats) + "\n\n");

    struct timespec ts2 = {0, 100000000};
    nanosleep(&ts2, NULL);

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to remove a read entry in shared table\n\n");

    remove_read_entry(trainID);

    return availableSeats;
}

void add_write_entry(int trainID) {
    struct timespec ts;
    pthread_mutex_lock(&tableMutex);
    tabEntry* p1 = NULL, *p2 = NULL;
    int a = 0;
    while ((a = activeQueries) >= MAX_QUERIES || (p1 = get_read_entry(trainID)) || (p2 = get_write_entry(trainID))) {
        ts = get_current_time();
        if (a >= MAX_QUERIES) {
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : SUSPENDED - MAX_QUERIES reached\n\n");
        }
        else if (p1) {
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : SUSPENDED - read entry for train " + to_string(trainID) + " found\n\n");
        }
        else {
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : SUSPENDED - write entry for train " + to_string(trainID) + " found\n\n");
        }
        pthread_cond_wait(&writePossible[trainID], &tableMutex);
        ts = get_current_time();
        print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : RESUMING - signal received\n\n");
    }
    activeQueries++;

    tabEntry* p = get_free_entry();
    p->trainID = trainID;
    p->queryType = 1;
    p->numOfThreads = 1;

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Write entry made\n\n");

    pthread_mutex_unlock(&tableMutex);
}

void remove_write_entry(int trainID) {
    struct timespec ts;
    pthread_mutex_lock(&tableMutex);
    tabEntry* p = get_write_entry(trainID);
    p->trainID = -1;
    activeQueries--;
    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Write entry removed. waking up all the threads waiting to read/write train " + to_string(trainID) + "\n\n");
    pthread_cond_broadcast(&readPossible[trainID]);
    pthread_cond_broadcast(&writePossible[trainID]);
    signal_free_cond();
    pthread_mutex_unlock(&tableMutex);
}

int book(int trainID, int k) {
    struct timespec ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to make a write entry in shared table\n\n");

    add_write_entry(trainID);

    int available = trainSeats[trainID];
    int booked = min(available, k);
    if(booked < 5) booked = 5;

    struct timespec ts2 = {0, 100000000};
    nanosleep(&ts2, NULL);

    trainSeats[trainID] -= booked;

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : booked " + to_string(booked) + " seats for the train " + to_string(trainID) + "\n\n");

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to remove a write entry in shared table\n\n");

    remove_write_entry(trainID);

    return booked;
}

void cancel(int trainID, int k) {
    struct timespec ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to make a write entry in shared table\n\n");

    add_write_entry(trainID);

    struct timespec ts2 = {0, 100000000};
    nanosleep(&ts2, NULL);

    trainSeats[trainID] += k;
    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : cancelled " + to_string(k) + " seats for the train " + to_string(trainID) + "\n\n");

    ts = get_current_time();
    print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to remove a write entry in shared table\n\n");

    remove_write_entry(trainID);
}

void* worker(void* arg) {
    int threadID = *((int*) arg);
    struct timespec ts;

    vector<pair<int, int> > bookings;
    random_device rd;
    mt19937 gen(rd());

    uniform_int_distribution<> queryDis(1, 3);
    uniform_int_distribution<> seatDis(5, 10); // Modified to ensure bookings and cancellations are between 5 and 10
    uniform_int_distribution<> trainDis(0, TRAINS - 1);

    while (true) {
        pthread_mutex_lock(&exitMutex);
        if (exitNow) {
            pthread_mutex_unlock(&exitMutex);
            break;
        }
        pthread_mutex_unlock(&exitMutex);

        // Introduce a delay of 25 milliseconds before sending each query
        ts = {0, 25000000}; // 25 milliseconds in nanoseconds
        nanosleep(&ts, NULL);

        int q = queryDis(gen);
        if (q == 1) {
            int trainToQuery = trainDis(gen);
            ts = get_current_time();
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to enquire train " + to_string(trainToQuery) + "\n\n");
            int available = enquire(trainToQuery);
            ts = get_current_time();
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Available seats in train " + to_string(trainToQuery) + " = " + to_string(available) + "\n\n");
        }
        else if (q == 2) {
            int trainToBook = trainDis(gen);
            int numSeats = seatDis(gen); // Generate random number of seats between 5 and 10
            if (numSeats < 5) {
                numSeats = 5; // Ensure at least 5 tickets are booked
            }
            ts = get_current_time();
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to book " + to_string(numSeats) + " seats in train " + to_string(trainToBook) + "\n\n");
            int booked = book(trainToBook, numSeats);
            bookings.push_back({trainToBook, booked});
        }
        else {
            if (bookings.size() == 0) {
                continue;
            }
            int idx = rand() % bookings.size();
            pair<int, int> booking = bookings[idx];
            bookings.erase(bookings.begin() + idx);
            int trainToCancel = booking.first;
            // Use seatDis for both booking and cancellation
            int numSeats = seatDis(gen);
            ts = get_current_time();
            print_output(to_string(ts.tv_sec) + " min, " + to_string(ts.tv_nsec%60) + " s : Attempting to cancel " + to_string(numSeats) + " seats in train " + to_string(trainToCancel) + "\n\n");
            cancel(trainToCancel, numSeats);
        }
    }

    pthread_exit(NULL);
}

void init() {
    activeQueries = 0;
    for (int i = 0; i < TRAINS; i++) {
        pthread_cond_init(&readPossible[i], NULL);
        pthread_cond_init(&writePossible[i], NULL);
    }
    for (int i = 0; i < MAX_QUERIES; i++) {
        sharedTable[i] = getNewEntry(-1, -1, -1);
    }
}

void cleanup() {
    pthread_mutex_destroy(&tableMutex);
    pthread_mutex_destroy(&exitMutex);
    for (int i = 0; i < TRAINS; i++) {
        pthread_cond_destroy(&readPossible[i]);
        pthread_cond_destroy(&writePossible[i]);
    }
    for (int i = 0; i < MAX_QUERIES; i++) {
        free(sharedTable[i]);
    }
}

void timer_handler(int signum) {
    pthread_mutex_lock(&exitMutex);
    exitNow = true;
    pthread_mutex_unlock(&exitMutex);
}

int main() {
    srand(time(NULL));
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    init();

    pthread_t threads[NUM_THREADS];
    int threadIDs[NUM_THREADS];

    pthread_mutex_init(&tableMutex, NULL);
    pthread_mutex_init(&exitMutex, NULL);

    for (int i = 0; i < NUM_THREADS; i++) {
        threadIDs[i] = i;
        pthread_create(&threads[i], NULL, worker, (void*) &threadIDs[i]);
        // Introduce a delay of 25 milliseconds between creating threads
        struct timespec ts = {0, 25000000}; // 25 milliseconds in nanoseconds
        nanosleep(&ts, NULL);
    }

    signal(SIGALRM, timer_handler);
    alarm(SIMULATION_TIME);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    cleanup();

    return 0;
}
