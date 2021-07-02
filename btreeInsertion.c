#include "btreeInsertion.h"

// Toda lógica da inserção em btree tá aqui
// Ignorem a bagunça em algumas funções (nomes em português, outros em inglês), vou deixar bunitin depois kkkkk

// Cria uma estrutura de chave promovida com dados de um registro. Útil para as inserções e splits
promotedKey *criarChave(record *newRecord, long leftChild, long rightChild) {
    promotedKey *chave = (promotedKey *)malloc(sizeof(promotedKey));
    chave->childs[0] = leftChild;
    chave->childs[1] = rightChild;
    chave->recordKey = newRecord;

    return chave;
}

/* Retorna o RRN de uma página filha para inserir uma chave.
   Faz esse processo até achar um nó folha */
long procuraPaginaPraInserir(record *newRecord, btPage *page) {
    record *pageRecord = NULL;
    for(int i = 0; i < page->numberOfKeys; i++) {
        pageRecord = page->records[i];
        if(newRecord->key == pageRecord->key)
            return -1; // chaves iguais, erro!
        else if(newRecord->key < pageRecord->key)
            return page->childs[i];
    }

    return page->childs[page->numberOfKeys];
}

// Busca uma posição dentro do vetor de registros da página para inserir a nova chave
int buscaPosicaoNoNode(promotedKey *newRecord, btPage *page) {
    int i = 0;
    for(i = 0; i < page->numberOfKeys; i++) {
        if(newRecord->recordKey->key < page->records[i]->key)
            break;
        else if(newRecord->recordKey->key == page->records[i]->key)
            return -1; // já foi inserido, ERRO
    }

    return i;
}

// Insere uma chave de forma ordenada em uma página
Errors insercaoOrdenadaNoNode(promotedKey *newRecord, btPage *page) {
    int posicao = buscaPosicaoNoNode(newRecord, page);
    if(posicao != -1) {
        for(int i = page->numberOfKeys-1; i >= posicao; i--) {
            page->records[i+1] = page->records[i];
            page->childs[i+2] = page->childs[i+1];
        }
        page->childs[posicao] = newRecord->childs[0];
        page->childs[posicao+1] = newRecord->childs[1];
        page->records[posicao]->key = newRecord->recordKey->key;
        page->records[posicao]->recordRRN = newRecord->recordKey->recordRRN;

        return true;
    }
    else { // já foi inserido, ERRO
        return false;
    }
}

// "Reinicia" uma página splittada para receber novamente as chaves organizadas
void clearSplittedPage(btPage *page) {
    for(int i = 0; i < MAXKEYS; i++) {
        page->records[i] = NULL;
        page->childs[i] = -1;
    }
    page->childs[MAXKEYS] = -1;
    page->numberOfKeys = (MAXKEYS+1)/2;
}

// Organiza e distribui as chaves nos nós splittados
// (em construção)
promotedKey *organizeKeys(promotedKey **auxArray, btPage *originalPage, btPage *newPage) {
    int meio = (MAXKEYS+1)/2;
    clearSplittedPage(originalPage); // limpa a página original antes de recolocar os elementos
    for(int i = 0; i < meio; i++) {
        originalPage->records[i] = auxArray[i]->recordKey;
        originalPage->childs[i] = auxArray[i]->childs[0];
        originalPage->childs[i+1] = auxArray[i]->childs[1];
    }
    int j = 0;
    for(int i = meio; i < (MAXKEYS+1); i++) {
        newPage->records[j] = auxArray[i]->recordKey;
        newPage->childs[j] = auxArray[i]->childs[0];
        newPage->childs[j+1] = auxArray[i]->childs[1];
        newPage->numberOfKeys++;
        j++;
    }

    return criarChave(newPage->records[0], newPage->childs[0], newPage->childs[1]); // retorna a primeira chave da nova página
}

// Cria um array auxiliar de chaves para ajudar no overflow e na promoção de uma chave
promotedKey **createArrayForKeys(btPage *page, promotedKey *newRecord) {
    promotedKey **auxArray = (promotedKey **)malloc((MAXKEYS+1)*sizeof(promotedKey *));

    int contadorPagina = 0;
    int contadorArray = 0;
    while(contadorArray < (MAXKEYS+1)) {
        if(page->records[contadorPagina]->key < newRecord->recordKey->key) {
            auxArray[contadorArray] = criarChave(page->records[contadorPagina],page->childs[contadorPagina],page->childs[contadorPagina+1]);
            contadorPagina++;
        }
        else if(page->records[contadorPagina]->key > newRecord->recordKey->key) {
            auxArray[contadorArray] = newRecord;
        }
        else {
            return NULL;
        }

        contadorArray++;
    }

    return auxArray;
}

// Remove a chave promovida para o overflow
void removePromotedKeyFromPage(btPage *page) {
    int meio = (MAXKEYS/2) + 1;
    for(int i = 1; i < meio; i--) {
        page->records[i-1] = page->records[i];
        page->childs[i-1] = page->childs[i];
        page->childs[i] = page->childs[i+1];
    }
}

// Função principal do split
// (em construção)
promotedKey *split(btPage *splitedPage, promotedKey *newRecord, FILE *fp) {
    promotedKey **auxArray = createArrayForKeys(splitedPage, newRecord);

    btPage *newPage = allocatePage();
    newPage->isLeaf = splitedPage->isLeaf;
    fseek(fp, 0, SEEK_END);
    long rrnNewPage = ftell(fp);
    
    promotedKey *keyPromoted = organizeKeys(auxArray, splitedPage, newPage);
    removePromotedKeyFromPage(newPage);

    // próx passo: definir o rrn dos filhos da chave promovida
    keyPromoted->childs[1] = rrnNewPage;

    writePageIntoFile(rrnNewPage, newPage, fp);

    free(auxArray);

    return keyPromoted;
}

// Cria uma nova página e seta ela como raiz caso houver overflow na antiga raiz
void createAndSetNewRoot(btPage *formerRoot, long rrnFormerRoot, promotedKey *promotedKey, FILE *fp) {
    // Aloca e prepara os dados da nova raiz
    btPage *newRoot = allocatePage();
    newRoot->isLeaf = false;
    promotedKey->childs[0] = rrnFormerRoot;
    insertInNode(promotedKey, newRoot, fp);

    // Escreve a raiz antiga no arquivo
    writePageIntoFile(rrnFormerRoot, formerRoot, fp);

    // Calcula o rrn da nova raiz, escreve ela no arquivo e no cabeçalho
    fseek(fp, 0, SEEK_END);
    long rrnNewRoot = ftell(fp);
    writeTreeHeader(fp, rrnNewRoot);
    writePageIntoFile(rrnNewRoot, newRoot, fp);
}

// Faz uma inserção em um nó qualquer
promotedKey *insertInNode(promotedKey *newRecord, btPage *page, FILE *fp) {
    if(page->numberOfKeys < MAXKEYS) { // Sem overflow
        // Checa se é possível inserir. Se sim, insere
        bool checkInsertion = insercaoOrdenadaNoNode(newRecord, page);
        if(checkInsertion) {
            page->numberOfKeys++;
            //printf("Insercao %d feita!\n", page->numberOfKeys);
            //printf("Insercao %d feita com sucesso\n", page->numberOfKeys);
        }
        else
            printf("O Registro ja existe!\n");
        
        return NULL;
    }
    else { // Com overflow
        // Faz o split e retorna a chave promovida
        promotedKey *promotedKey = split(page, newRecord, fp);

        return promotedKey;
    }
}

// Inserção recursiva. Vai investigando os nós até achar uma folha
// (meio em construção ainda)
promotedKey *recInserirBTree(promotedKey *newRecord, btPage *page, FILE *fp) {
    promotedKey *promotedKey = NULL;
    if(page->isLeaf) {
        promotedKey = insertInNode(newRecord, page, fp);
    }
    else {
        long rrnPageFilha = procuraPaginaPraInserir(newRecord->recordKey, page);
        if(rrnPageFilha == -1) {
            printf("O Registro ja existe!\n");
            return NULL;
        }
        btPage *pageFilha = getPage(rrnPageFilha, fp);
        promotedKey = recInserirBTree(newRecord, pageFilha, fp);
        if(promotedKey != NULL) {
            promotedKey->childs[0] = rrnPageFilha;
            
            promotedKey = insertInNode(promotedKey, page, fp);
        }

        writePageIntoFile(rrnPageFilha, pageFilha, fp);
    }

    return promotedKey;
}

// Função principal para inserção
Errors bTreeInsert(record *newRecord, btPage *root, FILE *fp) {
    //FILE *fp = fopen("btree.bin", "r+");
    //btPage *root = getOrCreateRoot(fp);
    long rrnRoot = getTreeHeader(fp);

    promotedKey *newRecordKey = criarChave(newRecord,-1,-1);
    promotedKey *promotedKey = recInserirBTree(newRecordKey, root, fp);
    if(promotedKey != NULL) {
        //printf("Entrou num if que nao devia\n");
        createAndSetNewRoot(root, rrnRoot, promotedKey, fp);
    }
    else {
        //printf("Escrevendo a raiz de volta\n");
        writePageIntoFile(rrnRoot, root, fp);
        writeTreeHeader(fp, rrnRoot);
    }

    //fclose(fp);

    return true;
}