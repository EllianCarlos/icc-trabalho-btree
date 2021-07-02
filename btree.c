#include "btree.h"

btPage *allocatePage()
{
    btPage *page = malloc(sizeof(PAGELENGTH));
    page->keys = (nodeKey **)malloc(MAXKEYS * sizeof(nodeKey *));
    page->childs = malloc(sizeof(long) * (MAXKEYS + 1));
    for (int i = 0; i < MAXKEYS; i++)
    {
        page->keys[i] = NULL;
        page->childs[i] = -1;
    }
    page->childs[MAXKEYS] = -1;
    page->numberOfKeys = 0;
    page->isLeaf = true;
    return page;
}

void deallocatePage(btPage *page)
{
    for (int i = 0; i < MAXKEYS; i++)
    {
        if (page->keys[i] != NULL)
            free(page->keys[i]);
    }
    free(page->keys);
    free(page->childs);
    free(page);
}

void writeTreeHeader(FILE *fp, long rootRRN)
{
    //Calcula espaço livre e escreve no cabeçalho da árvore, junto com o RRN do nó raíz
    fseek(fp, 0, SEEK_SET);
    fwrite(&rootRRN, sizeof(long), 1, fp);
    while (ftell(fp) != PAGESIZE)
    {
        fwrite(FREESPACE, 1, 1, fp);
    }
}

btPage *readPageFromFile(FILE *fp)
{
    //Aloca espaço para carregar página
    btPage *page = allocatePage();
    //Lê dados da página do arquivo
    fread(page->keys, sizeof(nodeKey), MAXKEYS, fp);
    fread(page->childs, sizeof(long), (MAXKEYS + 1), fp);
    fread(&page->numberOfKeys, sizeof(short), 1, fp);
    fread(&page->isLeaf, sizeof(bool), 1, fp);
    return page;
}

Errors writePageIntoFile(long rrn, btPage *page, FILE *fp)
{
    //Verifica se está tudo ok com os dados
    if (PAGESIZE <= rrn && !page)
    {
        //Encontra local para escrita baseado no RRN
        fseek(fp, rrn, SEEK_SET);
        //Escreve dados
        fwrite(page->keys, sizeof(nodeKey), MAXKEYS, fp);
        fwrite(page->childs, sizeof(long), (MAXKEYS + 1), fp);
        fwrite(&page->numberOfKeys, sizeof(short), 1, fp);
        fwrite(&page->isLeaf, sizeof(bool), 1, fp);
        //Atualiza valor de espaço livre na página
        for (int i = 0; i <= FREESPACEONPAGE; i++)
        {
            fwrite(FREESPACE, 1, 1, fp);
        }
        return true;
    }
    else
    {
        return false;
    }
    //Dica: você pode criar uma função que só lida com a escrita dos dados e chamar aqui
}

long getTreeHeader(FILE *fp)
{
    long rrnRoot = 0;
    //Carrega o cabeçalho da árvore, que está no início do arquivo
    fseek(fp, 0, SEEK_SET);
    fread(&rrnRoot, sizeof(long), 1, fp);
    return rrnRoot;
}

btPage *getPage(long RRN, FILE *fp)
{
    //Recupera uma página baseado no RRN
    fseek(fp, RRN, SEEK_SET);
    //Procura e carrega seus dados
    return readPageFromFile(fp);
}

btPage *createTree(FILE *fp)
{
    //Aloca espaço pra raiz
    btPage *root = allocatePage();
    //Escreve a raiz no cabeçalho
    writeTreeHeader(fp, PAGESIZE);
    //Escreve a pagina da raiz
    writePageIntoFile(PAGESIZE, root, fp);
    return root;
}

btPage *getOrCreateRoot(FILE *fp)
{
    //Verifica se a árvore já existe ou precisa criar uma nova
    long rrnRoot = getTreeHeader(fp);
    //Se a árvore não existir, cria ela
    if (rrnRoot == 0)
    {
        return createTree(fp);
    }
    //Se existir, só pega o RRN da raiz no cabeçalho e carrega sua página
    else
    {
        return getPage(rrnRoot, fp);
    }
    //Pode ser adaptada pra inserção e busca sem precisar de 2 funções
}

/*Returns rrn if key exist else return -1*/
long bTreeSelect(btPage *node, int key, FILE *fp)
{
    long result;
    int i;
    if (!node)
    {
        return -1;
    }
    else
    {
        for (i = 0; i < node->numberOfKeys; i++)
        {
            //Procura no nó atual se a chave existe
            if (key == node->keys[i]->key)
            {
                //Se encontrar a chave, retorna RRN dela
                return node->keys[i]->recordRRN;
            }
            //Se não existir, tenta procurar no filho adequado, recursivamente
            else if (key < node->keys[i]->key && !node->isLeaf)
            {
                btPage *aux = getPage(node->childs[i], fp);
                result = bTreeSelect(aux, key, fp);
                deallocatePage(aux);
                return result;
            }
        }
        if (!node->isLeaf && (node->keys[i] && key < node->keys[i]->key))
        {
            btPage *aux = getPage(node->childs[i], fp);
            result = bTreeSelect(aux, key, fp);
            deallocatePage(aux);
            return result;
        }
    }
    //Se não encontrar (chegar num nó folha e não estiver lá), retorna -1
    return -1;
}

nodeKey *createRecord(int key, long RNN)
{
    nodeKey *newRecord = (nodeKey *)malloc(sizeof(nodeKey));
    newRecord->key = key;
    newRecord->recordRRN = RNN;
    newRecord->childs[0] = -1;
    newRecord->childs[1] = -1;

    return newRecord;
}

void deleteRecord(nodeKey *recordToDelete)
{
    free(recordToDelete);
}