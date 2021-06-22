#ifndef UTILS_H_
#define UTILS_H_

#define OP_INSERT 1
#define OP_UPDATE 2
#define OP_SEARCH 3
#define OP_EXIT 4

typedef struct Errors {
    unsigned int* errorCode;
} Errors;

int getOperation();
void startToken();
int getNumberFromToken();
char* getNameFromToken();
char* getCourseFromToken();
double getGradeFromToken();
void exit();

#endif // UTILS_H_