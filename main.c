#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "list.h"
#include "utils.h"

#define NORMAL 0
#define VIP 1
#define ADMIN 2

#define NUM_OF_MESSAGES_PER_USER 10
#define ALL_MESSAGES 50
#define BUFFER_SIZE 20

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t full;
sem_t empty;

int user_count = 0;
int messages_send_count = 0;
int messages_removed_count = 0;
int messages_read_count = 0;
int clear_count = 0;

int running = 1;

char *CLEAR_QUEUE_MESSAGE = "Clear queue by Admin";

struct user_args {
    int user_type;
    int user_id;
    char *message;
};

//=======================DEFINITIONS==============================================
void onInit();

void onDestroy();

void *consumerFunction(void *arg);

void *userFunction(void *in_args);

void enqueueMessage(struct message *msg);

struct message *createMessage(struct user_args *args, int message_id);

void addUser(int type, char *message);

void addConsumer();

int clearBuffer();

char *typeNameFromID(int id);

void printStats(char *tag);

//=======================IMPLEMENTATIONS==============================================
void onInit() {
    resetPointers();
    sem_init(&full, 0, 0);
    sem_init(&empty, 0, BUFFER_SIZE);
}

void onDestroy() {
    running = 0;
    printStats("SUMMARY");
    sem_destroy(&full);
    sem_destroy(&empty);
}

void *consumerFunction(void *arg) {
    while (running) {
        struct message *next_message = NULL;

        sem_wait(&full);
        pthread_mutex_lock(&mutex);
        next_message = popHead();
        if (strcmp(next_message->content, CLEAR_QUEUE_MESSAGE) == 0) {
            clear_count++;
            // clear queue
            int count = numberOfElements();
            messages_removed_count += count;
            struct message *tmp = NULL;
            while (count != 0) {
                sem_wait(&full);
                tmp = popHead();
                free(tmp); // dealloc memory for message
                sem_post(&empty);
                count--;
            }
        }

        messages_read_count++;
        printMessage("MESSAGE READ", next_message->content);

        pthread_mutex_unlock(&mutex);
        sem_post(&empty);

        if (next_message != NULL) free(next_message);

        if ((messages_read_count + messages_removed_count) == ALL_MESSAGES) onDestroy();
    }
    return NULL;
}

void *userFunction(void *in_args) {
    struct user_args *args = (struct user_args *) in_args;
    for (int i = 0; i < NUM_OF_MESSAGES_PER_USER; ++i) {
        struct message *new_message = createMessage(args, i);
        if (new_message != NULL) {
            enqueueMessage(new_message);
        }
    }
    free(args);
}

void enqueueMessage(struct message *msg) {

    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (msg->user_type == VIP) {
        pushVip(msg);
    } else {
        pushNormal(msg);
    }

    messages_send_count++;

    pthread_mutex_unlock(&mutex);
    sem_post(&full);
}

struct message *createMessage(struct user_args *args, int message_id) {
    struct message *new_message = NULL;
    new_message = (struct message *) malloc(sizeof(struct message));


    if (new_message != NULL) {
        new_message->user_type = args->user_type;

        if (args->user_type == ADMIN && clearBuffer()) {
            new_message->content = "Clear queue by Admin";
            return new_message;
        } else {
            char *content = NULL;
            content = (char *) malloc(100 * sizeof(char));

            if (content != NULL) {
                sprintf(content, "**Message from userType: %s, userID: %d, msgID: %d, message: %s",
                        typeNameFromID(args->user_type), args->user_id, message_id, args->message);
                new_message->content = content;
            } else {
                logInt("UNABLE TO ALLOC MESSAGE STRING: ", message_id);
                free(new_message);
                return NULL;
            }
            return new_message;
        }
    } else {
        logInt("UNABLE TO ALLOC MESSAGE STRUCT: ", message_id);
        return NULL;
    }
}

void printStats(char *tag) {
    printStringWithTag("STATS", tag);
    printIntWithTag("COUNT_SEND", messages_send_count);
    printIntWithTag("COUNT_QUEUE", numberOfElements());
    printIntWithTag("COUNT_REMOVED ", messages_removed_count);
    printIntWithTag("COUNT READ", messages_read_count);
    printIntWithTag("COUNT CLEAR", clear_count);

    if (messages_send_count != (numberOfElements() + messages_removed_count + messages_read_count)) {
        printStringWithTag("MISSING MESSAGES!!!======================================================", tag);
        running = 0;
    }
}

char *typeNameFromID(int id) {
    if (id == NORMAL) {
        return "NORMAL";
    } else if (id == VIP) {
        return "VIP___";
    } else if (id == ADMIN) {
        return "ADMIN_";
    }
}

int clearBuffer() {
    return rand() % 7 == 0;
}

void addUser(int type, char *message) {
    // input args for thread
    struct user_args *args = NULL;
    args = (struct user_args *) malloc(sizeof(struct user_args *));

    if (args != NULL) {
        args->user_type = type;
        args->user_id = user_count;
        args->message = message;

        // creation of thread for user
        pthread_t t;
        pthread_create(&t, NULL, &userFunction, args);
        user_count++;
    } else {
        logInt("UNABLE TO ALLOC USER_ARGS STRUCT: ", user_count);
    }
}

void addConsumer() {
    pthread_t t;
    pthread_create(&t, NULL, &consumerFunction, NULL);
}

int testVIP() {
    onInit();
    addConsumer();
    addUser(NORMAL, "abc");
    addUser(NORMAL, "abc");
    addUser(VIP, "I am a VIP");
    addUser(VIP, "I am a VIP");
    addUser(NORMAL, "abc");
    sleep(2);
}

int testAdminClear() {
    onInit();
    addConsumer();
    addUser(NORMAL, "abc");
    addUser(NORMAL, "abc");
    addUser(VIP, "I am a vip");
    addUser(VIP, "I am a vip");
    addUser(ADMIN, "abc");
    sleep(2);
}

void testEmptyQueueRead() {
    onInit();
    addConsumer();
    sleep(5);
    onDestroy();
}

void testFullQueueWrite(){
    onInit();
    printIntWithTag("Buffer size", BUFFER_SIZE);
    addUser(NORMAL, "One");
    addUser(NORMAL, "Two");
    addUser(NORMAL, "Three");
    sleep(5);
    onDestroy();
}

int main() {
    srand(time(NULL));
    testVIP();
//    testAdminClear();
//    testEmptyQueueRead();
    testFullQueueWrite();
    return 0;
}