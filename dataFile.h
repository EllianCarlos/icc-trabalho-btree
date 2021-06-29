#ifndef DATAFILE_H_
#define DATAFILE_H_

#include <stdio.h>
#include <stdlib.h>
#include "student.h"

long appendAsFixedSize(char *, student s);
void getByRNN(long, char *, student *, int);
void readEverythingFromFile(FILE *, int);
int getNumberOfStudentsInFile(FILE *);
void readFileFromRange(FILE *, int, int);
void updateStudent(long, char *, student *);

#endif // DATAFILE_H_