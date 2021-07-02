#include "student.h"

student *allocateStudent() {
    student *st = (student *)malloc(sizeof(student));
    st->name = malloc(sizeof(char) * MAX_STRING_SIZE);
    st->surname = malloc(sizeof(char) * MAX_STRING_SIZE);
    st->course = malloc(sizeof(char) * MAX_STRING_SIZE);

    return st;
}

student *createStudent(int nUsp, char *name, char *surname, char *course, float grade)
{
    student *st = allocateStudent();

    strcpy(st->name, name);
    strcpy(st->surname, surname);
    strcpy(st->course, course);
    /*
    st->course = courseToSave;
    st->name = nameToSave;
    st->surname = surnameToSave;
    */
    st->nUsp = nUsp;
    st->grade = grade;

    return st;
}

void deleteStudent(student *st)
{
    free(st->course);
    free(st->surname);
    free(st->name);
    free(st);
}

void printStudent(student *st)
{
    printf("-------------------------------\n");
    printf("USP number: %d\nName: %s\nSurname: %s\nCourse: %s\nTest grade: %.2f\n", st->nUsp, st->name, st->surname, st->course, st->grade);
    printf("-------------------------------\n");
}

void printStudentArray(student *s, int n)
{
    for (int i = 0; i < n; ++i)
    {
        printStudent(&s[i]);
        if (i != n - 1)
            printf("\n\n");
    }
}