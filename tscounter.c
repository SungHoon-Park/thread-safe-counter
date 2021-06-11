#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct __counter_t {
    int value;
    pthread_mutex_t lock;
} counter_t;

unsigned int loop_cnt;
counter_t counter;

void init(counter_t *c) {
    c->value = 0;
    pthread_mutex_init(&c->lock, NULL);
}

void increment(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value++;
    pthread_mutex_unlock(&c->lock);
}

void decrement(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    c->value--;
    pthread_mutex_unlock(&c->lock);
}

int get(counter_t *c) {
    pthread_mutex_lock(&c->lock);
    int rc = c->value;
    pthread_mutex_unlock(&c->lock);
    return rc;
}

void *mythread(void *arg)
{
    char *letter = arg;
    int i;

    printf("%s: begin\n", letter);
    for (i = 0; i < loop_cnt; i++) {
        increment(&counter);
    }
    printf("%s: done\n", letter);
    return NULL;
}
                                                                             
int main(int argc, char *argv[])
{                    
    loop_cnt = atoi(argv[1]);

    init(&counter);

    pthread_t p1, p2;
    printf("main: begin [counter = %d]\n", get(&counter));
    pthread_create(&p1, NULL, mythread, "A"); 
    pthread_create(&p2, NULL, mythread, "B");
    // join waits for the threads to finish
    pthread_join(p1, NULL); 
    pthread_join(p2, NULL); 
    printf("main: done [counter: %d] [should be: %d]\n", get(&counter), loop_cnt * 2);
    return 0;
}


#semaphore

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
    int val;
    struct semid_ds *buf;
    ushort *array;
};

#define PATH "/home/jongchank/key"

int main(int argc, char *argv[])
{
    key_t key;
    int semid;
    struct sembuf s;
    union semun arg;

    key = ftok(PATH, 'z');
    if (key < 0) {
        perror(argv[0]);
        exit(1);
    }
    semid = semget(key, 1, 0600 | IPC_CREAT);
    if (semid < 0) {
        perror(argv[0]);
        exit(1);
    }

    printf("semid = %d\n", semid);

    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    /* Lock */
    s.sem_num = 0;
    s.sem_op = -1; 
    s.sem_flg = 0;
    semop(semid, &s, 1);

    /* Critical Section Starts */
    printf("getting sem\n");
    for (int i = 0; i < 10; i++) {
        printf("%s in critical section\n", argv[0]);
        sleep(1);
    }
    printf("releasing sem\n");
    /* Critical Section Ends */

    /* Unlock */
    s.sem_num = 0;
    s.sem_op = 1;
    s.sem_flg = 0;
    semop(semid, &s, 1);
    /*
    semctl(semid, 0, IPC_RMID, NULL);      
    */
    return 0;
}
