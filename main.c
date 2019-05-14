
#include <stdio.h>
#include <stdlib.h>

/**Strutture dati necessarie alla memorizzazione dei dati nei nodi del grafo*/

enum TypeNode {
    Var, App, Lam
};

struct RelativeNode;

struct IndirectEdge {
    struct Node *left;
    struct Node *right;
    int isAlive;//1 se è vivo 0 altrimenti
};
struct ListIndirectEdge {
    struct ListIndirectEdge *head;
    struct IndirectEdge *edge;
    struct ListIndirectEdge *next;
    int count;
};
struct Node {
    struct RelativeNode *relativeNode;
    struct Node *binder;
    struct Node *canonic;
    int isAlive;//1 se è vivo 0 altrimenti
    enum TypeNode type;
    struct ListIndirectEdge *indirectEdge;
};
struct ListNode {
    struct ListNode *head;
    struct Node *node;
    struct ListNode *next;
    int count;
};
struct RelativeNode {
    struct Node *cLeft;
    struct Node *cRight;
    struct ListNode *parentNodes;
};

/***************************************************************************************************/
/**
 * Inizializza l'elemento in coda alla lista passata in input
 * @param listNode puntatore alla coda della lista
 * @param head della lista
 */
void InitListNode(struct ListNode *listNode, struct ListNode *head) {
    listNode->head = head;
    listNode->node = malloc(sizeof(struct Node));
    listNode->next = malloc(sizeof(struct ListNode));
    listNode->count = 0;
};

/**
 * Inizializza l'elemento in coda alla lista passata in input
 * @param listNode puntatore alla coda della lista
 * @param head della lista
 */
void InitListIndirectEdge(struct ListIndirectEdge *listNode, struct ListIndirectEdge *head) {
    listNode->head = head;
    listNode->edge = malloc(sizeof(struct IndirectEdge));
    listNode->next = malloc(sizeof(struct ListIndirectEdge));
    listNode->count = 0;
};
/**
 * Inserisce un nuovo elemento di tipo Nodo in coda alla lista passata di tipo ListNode e inizializza la nuova coda della lista.
 * @param list puntatore alla coda della lista
 * @param prtNode puntatore dell'elemento da aggiundere alla lista
 * @return il puntatore alla nuova coda della lista
 */
struct ListNode *PushToListNode(struct ListNode *list, struct Node *prtNode) {
    list->node = prtNode;
    InitListNode(list->next, list->head);
    list->next->count = list->count + 1;
    return list->next;
};
/**
 * inizializza un nodo passato in input come nodo Variabile
 * @param node puntatore del nodo da inizializzare
 */
void InitVar(struct Node *node) {
    node->isAlive = 1;
    node->type = Var;
    node->relativeNode = malloc(sizeof(struct RelativeNode));
    node->relativeNode->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->relativeNode->parentNodes, node->relativeNode->parentNodes);
    node->binder = NULL;
    node->canonic = NULL;
    node->indirectEdge = malloc(sizeof(struct ListIndirectEdge));
    InitListIndirectEdge(node->indirectEdge, node->indirectEdge);
}

/**
 * inizializza un nodo passato in input come nodo Applicazione
 * @param node puntatore del nodo da inizializzare
 */
void InitApp(struct Node *node, struct Node *left, struct Node *right) {
    node->isAlive = 1;
    node->type = App;
    node->relativeNode = malloc(sizeof(struct RelativeNode));
    node->relativeNode->cLeft = left;
    node->relativeNode->cRight = right;
    node->relativeNode->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->relativeNode->parentNodes, node->relativeNode->parentNodes);
    node->binder = NULL;
    node->canonic = NULL;
    node->indirectEdge = malloc(sizeof(struct ListIndirectEdge));
    InitListIndirectEdge(node->indirectEdge, node->indirectEdge);
    left->relativeNode->parentNodes = PushToListNode(left->relativeNode->parentNodes, node);
    right->relativeNode->parentNodes = PushToListNode(right->relativeNode->parentNodes, node);
}

/**
 * inizializza un nodo passato in input come nodo Lambda Astrazione
 * @param node puntatore del nodo da inizializzare
 */
void InitLam(struct Node *node, struct Node *left, struct Node *right) {
    if (left->type != Var) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un Var\n");
        exit(1);
    }
    node->isAlive = 1;
    node->type = Lam;
    node->relativeNode = malloc(sizeof(struct RelativeNode));
    node->relativeNode->cLeft = left;
    node->relativeNode->cRight = right;
    node->relativeNode->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->relativeNode->parentNodes, node->relativeNode->parentNodes);
    node->binder = NULL;
    node->canonic = NULL;
    node->indirectEdge = malloc(sizeof(struct ListIndirectEdge));
    InitListIndirectEdge(node->indirectEdge, node->indirectEdge);
    left->binder = node;
    left->relativeNode->parentNodes = PushToListNode(left->relativeNode->parentNodes, node);
    right->relativeNode->parentNodes = PushToListNode(right->relativeNode->parentNodes, node);
}

/********************************************************************************************************/
/**
 * valorizza l'elemento in coda alla lista e inizializza la nuova coda della lista
 * @param list è il puntatore alla coda di una lista di tipo ListIndirectEdge
 * @param left Nodo left dell'arco indiretto
 * @param right Nodo right dell'arco indiretto
 * @return  il puntatore alla nuova coda della lista
 */
struct ListIndirectEdge *PushToListIndirectEdge(struct ListIndirectEdge *list, struct Node *left, struct Node *right) {
    list->edge->isAlive = 1;
    list->edge->left = left;
    list->edge->right = right;
    InitListIndirectEdge(list->next, list->head);
    list->next->count = list->count + 1;
    return list->next;
};

/**
 * Crea l'arco indiretto fra due nodi
 * @param left puntatore al nodo left da collegare con right
 * @param right puntatore al nodo right da collegare con left
 */
void CreateIndirectEdge(struct Node *left, struct Node *right) {
    left->indirectEdge = PushToListIndirectEdge(left->indirectEdge, left, right);
    right->indirectEdge = PushToListIndirectEdge(right->indirectEdge, left, right);
}
/**
 * Ricerca l'altro capo di un arco indiretto
 * @param indirectEdge puntatore all'arco indiretto in cui ricercare
 * @param source putatore a una delle due estremità dell'arco indiretto
 * @return il puntatore al nodo all'altro capo dell'arco o NULL se non trova il source nell'indirectEdge
 */
struct Node *FindOtherEndEdge(struct IndirectEdge *indirectEdge, struct Node *source) {
    if (source == indirectEdge->left)
        return indirectEdge->right;
    else if (source == indirectEdge->right)
        return indirectEdge->left;
    return NULL;//"Error! Impossible to find the target";
}
/**
 * Funzione utilizzata a scopo di test
 * stampa una lista di nodi specificando il tipo dei nodi ed eventualmente il tipo dei figli
 * nel caso Lam anche il tipo del binder del figlio sinistro
 * @param listNode
 */
void PrintListNode(struct ListNode *listNode) {
    printf("PrintListNode\n");
    printf("Numero nodi %d\n", listNode->count);
    struct ListNode *nodes = listNode->head;
    while (nodes->node->isAlive != 0) {

        printf("PrintListNode - Nodes\n");
        enum TypeNode x = nodes->node->type;
        if (nodes->node->type == Var)
            printf("VarNode -Type Var\n");
        if (nodes->node->type == App) {
            printf("AppNode -Type App\n");
            if (nodes->node->relativeNode->cRight->isAlive == 1)
                printf("CRight -Type %d\n", nodes->node->relativeNode->cRight->type);
            if (nodes->node->relativeNode->cLeft->isAlive == 1)
                printf("CLeft -Type %d\n", nodes->node->relativeNode->cLeft->type);
        }
        if (nodes->node->type == Lam) {
            printf("LamNode -Type Lam\n");
            if (nodes->node->relativeNode->cRight->isAlive == 1)
                printf("CRight -Type %d\n", nodes->node->relativeNode->cRight->type);
            if (nodes->node->relativeNode->cLeft->isAlive == 1) {
                printf("CLeft -Type %d\n", nodes->node->relativeNode->cLeft->type);
                struct Node *temp = nodes->node->relativeNode->cLeft->binder;

                if (temp->isAlive == 1)
                    printf("binder -Type %d\n", temp->type);
            }
            printf("\n\n");
        }
        nodes = nodes->next;
    }
}


/***************************************************************************************************/
/**
 * inserisce un nodo nella queue se rispetta delle condizioni
 * @param queue lista in cui appendere il nodo
 * @param d nodo sotto analisi dalla procedura chiamante
 * @param t nodo da analizzare e pushare nella queue
 * @param edge da killare se la procedura và a buon fine
 * @return a nuova coda della queue
 */
struct ListNode *Push(struct ListNode *queue, struct Node *d, struct Node *t, struct IndirectEdge *edge) {
    if (t->canonic == NULL) {
        t->canonic = d;
    } else if (t->canonic != d) {
        printf("Exit ramo Push->t.canonic != d \n");
        exit(1);//error
    }
    edge->isAlive = 0;
    return PushToListNode(queue, t);
}

/**
 * mette a confronto i 2 nodi passati in input ed esegue delle procedure ad hoc
 * tali procedure determinano se l'algoritmo termina fallendo
 * oppure creano gli archi indiretti necessari per il confronto dei nodi figli
 * @param d puntatore ad un nodo il quale deve essere messo a confronto con h
 * @param h puntatore ad un nodo il quale deve essere messo a confronto con d
 */
void Propagate(struct Node *d, struct Node *h) {
    if (d->type == Var) {
        if (d->binder == NULL || h->binder == NULL) {
            if (d!=h){//confronto i puntatori per vedere se d e h puntano allo stesso nodo
                printf("Exit ramo Propagate->d->binder == NULL || h->binder == NULL\n");
                exit(1);
            }
        } else if (d->binder->canonic != h->binder->canonic) {
            printf("Exit ramo Propagate->d->binder->canonic!=h->binder->canonic\n");
            exit(1);//different canonic
        }
    } else {
        // il controllo che siano effettiavamente dello stesso tipo-nodo viene fatto nella procedura chiamnte ossia Kill
        printf("CreateIndirectEdge ramo Propagate \n");
        CreateIndirectEdge(d->relativeNode->cLeft, h->relativeNode->cLeft);
        CreateIndirectEdge(d->relativeNode->cRight, h->relativeNode->cRight);
    }
    h->isAlive = 0;
}

/**
 * fornito un nodo in input la funzione si richiama ricorsivamente sui genitori del nodo ed elabora gli archi indiretti
 * @param d
 */
void Kill(struct Node *d) {
    printf("Kill Node Test, %d\n", d->type);
    struct ListNode *queue = malloc(sizeof(struct ListNode));
    InitListNode(queue, queue);

    if (d->canonic == NULL)
        d->canonic = d;
    else {
        printf("Exit ramo Kill->d->canonic == NULL---- Else\n");
        exit(1);// todo 1 errore
    }

    if (d->relativeNode->parentNodes != NULL) {
        struct ListNode *parents = d->relativeNode->parentNodes->head;
        for (int i = 0; i < d->relativeNode->parentNodes->count; ++i) {
            if (parents->node->isAlive != 0)
                Kill(parents->node);
            parents = parents->next;
        }
    }

    if (d->indirectEdge != NULL) {
        struct ListIndirectEdge *indirectEdges = d->indirectEdge->head;
        for (int i = 0; i < d->indirectEdge->count; ++i) {
            if (indirectEdges->edge->isAlive != 0) {
                struct Node *t = FindOtherEndEdge(indirectEdges->edge, d);
                queue = Push(queue, d, t, indirectEdges->edge);
            }
            indirectEdges = indirectEdges->next;
        }
    }

    struct ListNode iterQueue = *queue->head;
    int count = 0;
    while (count < queue->count) {
        struct Node *h = iterQueue.node;
        if (d->type != h->type) {
            printf("Exit ramo Kill-> d->type != h.type\n");
            exit(1);
        }
        if (h->relativeNode->parentNodes != NULL) {
            struct ListNode *parentsH = h->relativeNode->parentNodes->head;
            for (int i = 0; i < h->relativeNode->parentNodes->count; ++i) {
                if (parentsH->node->isAlive != 0)
                    Kill(parentsH->node);
                parentsH = parentsH->next;
            }
        }
        if (h->indirectEdge != NULL) {
            struct ListIndirectEdge *indirectEdgesH = h->indirectEdge->head;
            for (int i = 0; i < h->indirectEdge->count; ++i) {
                if (indirectEdgesH->edge->isAlive != 0) {
                    struct Node *f = FindOtherEndEdge(indirectEdgesH->edge, d);
                    queue = Push(queue, d, f, indirectEdgesH->edge);
                }
                indirectEdgesH = indirectEdgesH->next;
            }
        }

        Propagate(d, h);
        iterQueue = *iterQueue.next;
        count++;
    }
    d->isAlive = 0;
}

/**
 * procedura che visita tutti i nodi di una lista (se sono vivi) e li uccide tramite la procedura Kill
 * @param listNode lista di nodi da visitare
 */
void BlindSharingCheck(struct ListNode *listNode) {
    struct ListNode *nodes = listNode->head;
    for (int i = 0; i < listNode->count; ++i) {
        if (nodes->node->canonic == NULL)
            Kill(nodes->node);
        nodes = nodes->next;
    }
}


/***************************************************************************************************/
int main() {
    printf("Start Test\n");
    struct ListNode *nodes = malloc(sizeof(struct ListNode));
    InitListNode(nodes, nodes);

    struct Node node1Var;
    InitVar(&node1Var);
    struct Node node2Var;
    InitVar(&node2Var);
    struct Node node1App;
    InitApp(&node1App, &node1Var, &node2Var);
    struct Node node3Var;
    InitVar(&node3Var);
    struct Node node1Lam;
    InitLam(&node1Lam, &node3Var, &node1App);

    nodes = PushToListNode(nodes, &node1Var);
    nodes = PushToListNode(nodes, &node2Var);
    nodes = PushToListNode(nodes, &node1App);
    nodes = PushToListNode(nodes, &node3Var);
    nodes = PushToListNode(nodes, &node1Lam);

    struct Node node11App;
    struct Node node31Var;
    InitVar(&node31Var);
    struct Node node11Lam;

    InitApp(&node11App, &node1Var, &node2Var);
    InitLam(&node11Lam, &node31Var, &node11App);

    nodes = PushToListNode(nodes, &node11App);
    nodes = PushToListNode(nodes, &node31Var);
    nodes = PushToListNode(nodes, &node11Lam);

    CreateIndirectEdge(&node1Lam, &node11Lam);//creo un arco indiretto fra le 2 root dei grafi da confrontare

    PrintListNode(nodes);
    printf("\nStart Test PARTE2\n\n");

    BlindSharingCheck(nodes);

    printf("END, World!\n");
    return 0;
}