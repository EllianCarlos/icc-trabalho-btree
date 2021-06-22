#ifndef BTREE
#define BTREE

#include <utils.h>
#include <stdio.h>

#define PAGESIZE 4096
#define TREEHEADER PAGESIZE
#define MAXKEYS 204
#define AUXFIELDSSIZEONPAGE (2 + 1) /∗numberofkeysand”isleaf”bool∗/
#define FREESPACEONPAGE (PAGESIZE−((MAXKEYS∗4) + (MAXKEYS∗8) + ((MAXKEYS + 1)∗8) + 3))

typedef struct record
{
    int key;
    long recordRRN;
} record;

typedef struct page
{
    record *records;
    long *childs;
    short numberOfKeys;
    char isLeaf;
} btPage;

typedef struct promotedkey
{
    int key;
    long recordRRN;
    long childs[2];
} promotedKey;

btPage* getOrCreateRoot(FILE *);
btPage* getRoot(FILE);
Errors bTreeInsert(record *, btPage*, FILE*);
long bTreeSelect(btPage*, int, FILE*);
#endif