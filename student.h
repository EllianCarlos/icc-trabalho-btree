#ifndef STUDENT_H_
#define STUDENT_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_STRING_SIZE 255 

typedef struct student {
    int nUsp;
    char* name;
    char* surname;
    char* course;
    float grade;
} student;

student* createStudent(int, char*, char*, char*, float);
void printStudent(student*);
void printStudentArray(student*, int n);
void deleteStudent(student*);

#endif // STUDENT_H_  