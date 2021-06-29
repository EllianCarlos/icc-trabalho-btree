#ifndef UTILS_H_
#define UTILS_H_

#include "student.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define OP_INSERT 1
#define STRING_INSERT "insert"
#define OP_UPDATE 2
#define STRING_UPDATE "update"
#define OP_SEARCH 3
#define STRING_SEARCH "search"
#define OP_EXIT 4
#define STRING_EXIT "exit"

#define Errors bool

int getOperation(char *);
int getOperationNumber(char *);
student *getStudentFromWord(char *);
student *getStudentFromLine(char *);
int getNUspFromLine(char *);
void exitSystem();

#endif // UTILS_H_