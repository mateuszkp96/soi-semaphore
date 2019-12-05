//
// Created by mateusz on 12/5/19.
//

#ifndef SEMAPHOR_UTILS_H
#define SEMAPHOR_UTILS_H

#include <stdio.h>

#define DEBUG 1

void logString(char *tag, char *content) {
#if DEBUG
    printf("LOG: %s \n\t %s\n", tag, content);
#endif
}

void logInt(char *tag, int content) {
#if DEBUG
    printf("%s \n\t %d\n", tag, content);
#endif
}

void printIntWithTag(char *tag, int content) {
    printf("%s \n\t %d\n", tag, content);
}

void printStringWithTag(char *tag, char *content) {
    printf("%s \n\t %s\n", tag, content);
}

void printMessage(char *tag, char *content) {
    printf("%s \t %s\n", tag, content);
}

#endif //SEMAPHOR_UTILS_H
