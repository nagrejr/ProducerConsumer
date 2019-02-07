#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUF_SIZE 5

typedef struct {
    int buf[BUF_SIZE];
    size_t len;
    pthread_mutex_t mutex;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
}buffer_t;

void* producer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;

    while(1) {
#ifdef UNDERFLOW
        sleep(rand() % 3);
#endif

        pthread_mutex_lock(&buffer->mutex);

        if(buffer->len == BUF_SIZE) { 
            pthread_cond_wait(&buffer->can_produce, &buffer->mutex);
        }

        int t = rand()%50;
        printf("Produced: %d\n", t);

        buffer->buf[buffer->len] = t;
        ++buffer->len;

        pthread_cond_signal(&buffer->can_consume);
        pthread_mutex_unlock(&buffer->mutex);
    }

    return NULL;
}

void* consumer(void *arg) {
    buffer_t *buffer = (buffer_t*)arg;

    while(1) {
#ifdef OVERFLOW
        sleep(rand() % 3);
#endif
        pthread_mutex_lock(&buffer->mutex);

        if(buffer->len == 0) { 
            pthread_cond_wait(&buffer->can_consume, &buffer->mutex);
        }

        --buffer->len;
        printf("Consumed: %d\n", buffer->buf[buffer->len]);

        pthread_cond_signal(&buffer->can_produce);
        pthread_mutex_unlock(&buffer->mutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) {
    buffer_t buffer = {
        .len = 0,
        .mutex = PTHREAD_MUTEX_INITIALIZER,
        .can_produce = PTHREAD_COND_INITIALIZER,
        .can_consume = PTHREAD_COND_INITIALIZER
    };

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer, (void*)&buffer);
    pthread_create(&cons, NULL, consumer, (void*)&buffer);
    pthread_join(prod, NULL);
    pthread_join(cons, NULL);
    return 0;
}
