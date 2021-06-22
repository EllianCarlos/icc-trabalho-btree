#ifndef READ_FROM_FIXED_SIZE_FILE_H_
#define READ_FROM_FIXED_SIZE_FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include "student.h"

void readEverythingFromFile(FILE*,int);
int getNumberOfStudentsInFile(FILE *);
void readFileFromRange(FILE*, int, int);

#endif // READ_FROM_FIXED_SIZE_FILE_H_