#ifndef STUDENT_H_
#define STUDENT_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "limiters.h"

typedef struct student
{
    int nUsp;
    char *name;
    char *surname;
    char *course;
    float grade;
} student;

typedef struct students
{
    student *stArray;
    int len;
} students;

student *createStudent(int, char *, char *, char *, float);
void printStudent(student *);
void printStudentArray(student *, int n);
void deleteStudent(student *);
void deleteStudents(students *);

#endif // STUDENT_H_