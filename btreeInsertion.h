#include "btree.h"

promotedKey *criarChave(record *newRecord);
long procuraPaginaPraInserir(record *newRecord, btPage *page);
int buscaPosicaoNoNode(record *newRecord, btPage *page);
Errors insercaoOrdenadaNoNode(record *newRecord, btPage *page);
promotedKey *convertRecordToKey(record *r, long child1, long child2);
void organizeKeys(promotedKey **auxArray, btPage *originalPage, btPage *newPage);
promotedKey **createArrayForKeys(btPage *page, record *newRecord);
void freeArrayForKeys(promotedKey **auxArray);
void split(btPage *splitedPage, record *newRecord, FILE *fp);
promotedKey *inserirEmFolha(record *newRecord, btPage *page, FILE *fp);
promotedKey *recInserirBTree(record *newRecord, btPage *page, FILE *fp);
Errors inserirBTree(record *newRecord);