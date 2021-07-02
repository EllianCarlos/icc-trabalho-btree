#include "btree.h"

promotedKey *criarChave(record *newRecord, long leftChild, long rightChild);
long procuraPaginaPraInserir(record *newRecord, btPage *page);
int buscaPosicaoNoNode(promotedKey *newRecord, btPage *page);
Errors insercaoOrdenadaNoNode(promotedKey *newRecord, btPage *page);
void clearSplittedPage(btPage *page);
promotedKey *organizeKeys(promotedKey **auxArray, btPage *originalPage, btPage *newPage);
promotedKey **createArrayForKeys(btPage *page, promotedKey *newRecord);
void removePromotedKeyFromPage(btPage *page);
promotedKey *split(btPage *splitedPage, promotedKey *newRecord, FILE *fp);
void createAndSetNewRoot(btPage *formerRoot, long rrnFormerRoot, promotedKey *keyPromoted, FILE *fp);
promotedKey *insertInNode(promotedKey *newRecord, btPage *page, FILE *fp);
promotedKey *recInserirBTree(promotedKey *newRecord, btPage *page, FILE *fp);
Errors bTreeInsert(record *newRecord, btPage *root, FILE *fp);