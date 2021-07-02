#ifndef DATAFILE_H_
#define DATAFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include "student.h"

long appendAsFixedSize(FILE *, student *s);
void getByRNN(long, FILE *);
void readEverythingFromFile(FILE *, int);
int getNumberOfStudentsInFile(FILE *);
void readFileFromRange(FILE *, int, int);
void updateStudent(long, FILE *, student *);

#endif // DATAFILE_H_