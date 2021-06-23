#ifndef BTREE
#define BTREE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "utils.h"

#define PAGESIZE 4096
#define TREEHEADER PAGESIZE
// #define MAXKEYS 204
#define MAXKEYS 4
#define AUXFIELDSSIZEONPAGE (2 + 1) // number of keys and ”is leaf” bool
#define FREESPACEONPAGE (PAGESIZE - ((MAXKEYS * 4) + (MAXKEYS * 8) + ((MAXKEYS + 1) * 8) + 3))
#define PAGELENGTH ((MAXKEYS * 4) + (MAXKEYS * 8) + ((MAXKEYS + 1) * 8) + 3)
#define RECORDLENGTH ((MAXKEYS * 4) + (MAXKEYS * 8))
#define FREESPACE ""

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
    bool isLeaf;
} btPage;

typedef struct promotedkey
{
    int key;
    long recordRRN;
    long childs[2];
} promotedKey;

btPage *getOrCreateRoot(FILE *);
btPage *getRoot(FILE);
Errors bTreeInsert(record *, btPage *, FILE *);
long bTreeSelect(btPage *, int, FILE *);
#endif