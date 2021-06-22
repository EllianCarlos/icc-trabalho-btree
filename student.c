#include "student.h"

student *createStudent(int nUsp, char *name, char *surname, char *course, float grade)
{
    student *st = malloc(sizeof(student));
    char *nameToSave = malloc(sizeof(char) * MAX_STRING_SIZE);
    char *surnameToSave = malloc(sizeof(char) * MAX_STRING_SIZE);
    char *courseToSave = malloc(sizeof(char) * MAX_STRING_SIZE);
    strcpy(nameToSave, name);
    strcpy(surnameToSave, surname);
    strcpy(courseToSave, course);
    st->course = courseToSave;
    st->name = nameToSave;
    st->surname = surnameToSave;
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