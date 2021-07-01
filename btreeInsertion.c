#include "btreeInsertion.h"

// Toda lógica da inserção em btree tá aqui
// Ignorem a bagunça em algumas funções (nomes em português, outros em inglês), vou deixar bunitin depois kkkkk

// Cria uma chave com dados de um registro
nodeKey *criarChave(nodeKey *newRecord) {
    nodeKey *chave = (nodeKey *)malloc(sizeof(nodeKey));
    chave->childs[0] = -1;
    chave->childs[1] = -1;
    chave->key = newRecord->key;
    chave->recordRRN = newRecord->recordRRN;

    return chave;
}

/* Retorna o RRN de uma página filha para inserir uma chave.
   Faz esse processo até achar um nó folha */
long procuraPaginaPraInserir(nodeKey *newRecord, btPage *page) {
    nodeKey *pageRecord = NULL;
    for(int i = 0; i < page->numberOfKeys; i++) {
        pageRecord = page->keys[i];
        if(newRecord->key == pageRecord->key)
            return -1; // chaves iguais, erro!
        else if(newRecord->key < pageRecord->key)
            return page->childs[i];
    }

    return page->childs[page->numberOfKeys];
}

// Busca uma posição dentro do vetor de registros da página para inserir a nova chave
int buscaPosicaoNoNode(nodeKey *newRecord, btPage *page) {
    int i = 0;
    for(i = 0; i < page->numberOfKeys; i++) {
        if(newRecord->key < page->keys[i]->key)
            break;
        else if(newRecord->key == page->keys[i]->key)
            return -1; // já foi inserido, ERRO
    }

    return i;
}

// Insere uma chave de forma ordenada em uma página
Errors insercaoOrdenadaNoNode(nodeKey *newRecord, btPage *page) {
    int posicao = buscaPosicaoNoNode(newRecord, page);
    if(posicao != -1) {
        for(int i = page->numberOfKeys-1; i >= posicao; i--) {
            page->keys[i+1] = page->keys[i];
            page->childs[i+2] = page->childs[i+1];
        }
        page->keys[posicao] = newRecord;
        page->childs[posicao] = newRecord->childs[0];
        page->childs[posicao+1] = newRecord->childs[1];

        return true;
    }
    else { // já foi inserido, ERRO
        return false;
    }
}

// "Reinicia" uma página splittada para receber novamente as chaves organizadas
void clearSplittedPage(btPage *page) {
    for(int i = 0; i < MAXKEYS; i++) {
        page->keys[i] = NULL;
        page->childs[i] = -1;
    }
    page->childs[MAXKEYS] = -1;
    page->numberOfKeys = (MAXKEYS+1)/2;
}

// Organiza e distribui as chaves nos nós splittados
// (em construção)
nodeKey *organizeKeys(nodeKey **auxArray, btPage *originalPage, btPage *newPage) {
    int meio = (MAXKEYS+1)/2;
    clearSplittedPage(originalPage); // limpa a página original antes de recolocar os elementos
    for(int i = 0; i < meio; i++) {
        originalPage->keys[i] = auxArray[i];
        originalPage->childs[i] = auxArray[i]->childs[0];
        originalPage->childs[i+1] = auxArray[i]->childs[1];
    }
    int j = 0;
    for(int i = meio; i < (MAXKEYS+1); i++) {
        newPage->keys[j] = auxArray[i];
        newPage->childs[j] = auxArray[i]->childs[0];
        newPage->childs[j+1] = auxArray[i]->childs[1];
        newPage->numberOfKeys++;
        j++;
    }

    return newPage->keys[0]; // retorna a primeira chave da nova página
}

// Cria um array auxiliar de chaves para ajudar no overflow e na promoção de uma chave
nodeKey **createArrayForKeys(btPage *page, nodeKey *newRecord) {
    nodeKey **auxArray = (nodeKey **)malloc((MAXKEYS+1)*sizeof(nodeKey *));

    int contadorPagina = 0;
    int contadorArray = 0;
    while(contadorArray < (MAXKEYS+1)) {
        if(page->keys[contadorPagina]->key < newRecord->key) {
            auxArray[contadorArray] = page->keys[contadorPagina];
            contadorPagina++;
        }
        else if(page->keys[contadorPagina]->key > newRecord->key) {
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
        page->keys[i-1] = page->keys[i];
        page->childs[i-1] = page->keys[i]->childs[0];
        page->childs[i] = page->keys[i]->childs[1];
    }
}

// Função principal do split
// (em construção)
nodeKey *split(btPage *splitedPage, nodeKey *newRecord, FILE *fp) {
    nodeKey **auxArray = createArrayForKeys(splitedPage, newRecord);

    btPage *newPage = allocatePage();
    newPage->isLeaf = splitedPage->isLeaf;
    fseek(fp, 0, SEEK_END);
    long rrnNewPage = ftell(fp);
    
    nodeKey *keyPromoted = organizeKeys(auxArray, splitedPage, newPage);
    removePromotedKeyFromPage(newPage);

    // próx passo: definir o rrn dos filhos da chave promovida
    keyPromoted->childs[1] = rrnNewPage;

    writePageIntoFile(rrnNewPage, newPage, fp);

    free(auxArray);

    return keyPromoted;
}

// Cria uma nova página e seta ela como raiz caso houver overflow na antiga raiz
void createAndSetNewRoot(btPage *formerRoot, long rrnFormerRoot, nodeKey *promotedKey, FILE *fp) {
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
nodeKey *insertInNode(nodeKey *newRecord, btPage *page, FILE *fp) {
    if(page->numberOfKeys < MAXKEYS) { // Sem overflow
        // Checa se é possível inserir. Se sim, insere
        bool checkInsertion = insercaoOrdenadaNoNode(newRecord, page);
        if(checkInsertion) {
            page->numberOfKeys++;
        }
        else
            printf("O Registro ja existe!\n");
        
        return NULL;
    }
    else { // Com overflow
        // Faz o split e retorna a chave promovida
        nodeKey *promotedKey = split(page, newRecord, fp);

        return promotedKey;
    }
}

// Inserção recursiva. Vai investigando os nós até achar uma folha
// (meio em construção ainda)
nodeKey *recInserirBTree(nodeKey *newRecord, btPage *page, FILE *fp) {
    nodeKey *promotedKey = NULL;
    if(page->isLeaf) {
        promotedKey = insertInNode(newRecord, page, fp);
    }
    else {
        long rrnPageFilha = procuraPaginaPraInserir(newRecord, page);
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
Errors inserirBTree(nodeKey *newRecord) {
    FILE *fp = fopen("btree.bin", "r+");
    btPage *root = getOrCreateRoot(fp);
    long rrnRoot = getTreeHeader(fp);

    nodeKey *promotedKey = recInserirBTree(newRecord, root, fp);
    if(promotedKey != NULL) {
        createAndSetNewRoot(root, rrnRoot, promotedKey, fp);
    }
    else {
        writePageIntoFile(rrnRoot, root, fp);
        writeTreeHeader(fp, rrnRoot);
    }

    fclose(fp);
}