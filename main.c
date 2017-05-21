/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: hesham
 *
 * Created on May 12, 2017, 10:48 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#define SIZE 10
int AR = 0;
int WR = 0;
int AW = 0;
int WW = 0;
int data[SIZE];
pthread_mutex_t lock;
pthread_cond_t okRead;
pthread_cond_t okWrite;

/*
 * 
 */
int search(int v) {
    int i, vr = 0;
    for (i = 0; i < SIZE; i++) {
        if (v == data[i]) {
            vr = i;
        }
        break;
    }
    return vr;
}

int accessData(int mode) {
    int v = 0, i;
    switch (mode) {
        case 0:
            for (i = 0; i < SIZE; i++) {
                if (data[i] % 2 == 0) {
                    v = data[i];
                }
            }
            break;
        case 1:
            for (i = 0; i < SIZE; i++) {
                data[i] += i * i * 2 + 5;
            }
            break;
    }
    return v;
}

void *reader(void* arg) {
    pthread_mutex_lock(&lock);
    //    sleep(2);
    while ((AW + WW) > 0) {
        WR++;
        printf("%s: writer(s) active: %d; waiting readers: %d\n", (char*) arg, AW, WR);
        pthread_cond_wait(&okRead, &lock);
        WR--;
        printf("%s: writer(s) done; waiting readers: %d\n", (char*) arg, WR);
    }
    AR++;
    printf("%s: reader(s) active:%d; waiting writers: %d\n", (char*) arg, AR, WW);
    sleep(2);
    pthread_mutex_unlock(&lock);
    int v = accessData(0);
    printf("%s: reading: %d\n", (char*) arg, v);
    sleep(2);
    pthread_mutex_lock(&lock);
    AR--;
    if ((AR == 0) && (WW > 0)) {
        printf("%s: done reading; waiting writers: %d\n", (char*) arg, WW);
        sleep(1);
        pthread_cond_signal(&okWrite);
    }
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);

}

void *writer(void * arg) {
    pthread_mutex_lock(&lock);
    while ((AW + AR) > 0) {
        WW++;
        printf("%s: reader(s) active: %d; waiting writers: %d\n", (char*) arg, AR, WW);
        pthread_cond_wait(&okWrite, &lock);
        WW--;
        printf("%s: Reader(s) done; waiting writers: %d\n", (char*) arg, WW);
    }
    AW++;
    printf("%s: writer(s) active:%d; waiting readers: %d\n", (char*) arg, AW, WR);
    sleep(3);
    pthread_mutex_unlock(&lock);
    int v = accessData(1);
    printf("%s: writing: %d\n", (char*) arg, data[SIZE - 1]);
    sleep(1);
    pthread_mutex_lock(&lock);
    AW--;
    if (WW > 0) {
        printf("%s: still writers; waiting writers: %d\n", (char*) arg, WW);
        sleep(1);
        pthread_cond_signal(&okWrite);
    } else if (WR > 0) {
        printf("%s: no writers; waiting readers: %d\n", (char*) arg, WR);
        sleep(1);
        pthread_cond_broadcast(&okRead);

    }
    pthread_mutex_unlock(&lock);
    pthread_exit(NULL);

}

int main(int argc, char** argv) {

    pthread_t thread1, thread2, thread3, thread4, thread5;
    char* msg1 = "Writer1";
    char* msg4 = "Writer2";
    char* msg2 = "Reader1";
    char* msg3 = "Reader2";
    char* msg5 = "Reader3";
    void * s1;
    void * s2;
    void * s3;
    void * s4;
    void * s5;
    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&okRead, NULL);
    pthread_cond_init(&okWrite, NULL);
    int r1 = pthread_create(&thread1, NULL, &writer, (void *) msg1);
    int r2 = pthread_create(&thread2, NULL, &reader, (void *) msg2);
    int r3 = pthread_create(&thread3, NULL, &reader, (void *) msg3);
    int r4 = pthread_create(&thread4, NULL, &writer, (void *) msg4);
    int r5 = pthread_create(&thread5, NULL, &reader, (void *) msg5);
    if ((r1 != 0) || (r2 != 0)) {
        printf("error creating threads\n");
        exit(EXIT_FAILURE);
    }
    pthread_join(thread1, &s1);
    pthread_join(thread2, &s2);
    pthread_join(thread2, &s3);
    pthread_join(thread4, &s4);
    pthread_join(thread5, &s5);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&okRead);
    pthread_cond_destroy(&okWrite);

    return (EXIT_SUCCESS);
}

