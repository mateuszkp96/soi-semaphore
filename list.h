//
// Created by mateusz on 12/3/19.
//

#ifndef SEMAPHOR_LIST_H
#define SEMAPHOR_LIST_H

#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

struct message {
    struct message *next;
    char *content;
    int user_type;
};

struct message *head, *tail, *vip_head;

void resetPointers() {
    head = NULL;
    tail = NULL;
    vip_head = NULL;
}

void pushNormal(struct message *msg) {
    if (tail != NULL) { // messages in queue
        tail->next = msg;
        tail = msg;
        tail->next = NULL;
        return;
    } else { // no messages in queue
        // add to head
        head = msg;
        tail = msg;
        tail->next = NULL;
        return;
    }
}

void pushVip(struct message *msg) {

    if (head == NULL) { // no messages in queue
        head = msg;
        head->next = NULL;
        vip_head = head;
        tail = head;
        return;
    } else {
        // messages in queue
        if (vip_head == NULL) { // no vip messages
            msg->next = head;
            head = msg;
            vip_head = head;
            return;
        } else { // vip messages in queue
            if (vip_head == tail) {
                // vip message is last message
                msg->next = NULL;
                vip_head->next = msg;
                vip_head = msg;
                tail = msg;
                return;
            }
            else {
                // vip message is not last message
                msg->next = vip_head->next;
                vip_head->next = msg;
                vip_head = msg;
                return;
            }

        }
    }
}

struct message *popHead() {
    struct message *tmp = NULL;
    if (head != NULL) {
        if (head->next == NULL) { // only head in queue
            tmp = head;
            resetPointers();
            return tmp;
        }
        // not only head in queue
        tmp = head;
        if (head == vip_head) { // only one message vip
            vip_head = NULL;
            head = head->next;
            return tmp;
        } else {
            // head is not vip_head pointer
            head = head->next;
            return tmp;
        }
    }
    return NULL;
}

void printList() {
    struct message *tmp = head;
    printStringWithTag("BUFFER_PRINT", "");
    while (tmp != NULL) {
        printStringWithTag("\tBUFFER_MESSAGE", tmp->content);
        tmp = tmp->next;
    }
    printStringWithTag("", "");
}

int numberOfElements() {
    struct message *tmp = head;
    int count = 0;

    while (tmp != NULL) {
        count++;
        tmp = tmp->next;
    }
    return count;
}

#endif //SEMAPHOR_LIST_H
