//
// Created by andrea on 13/05/19.
//
#include <stdio.h>
#include <stdlib.h>

/**Strutture dati necessarie alla memorizzazione dei dati nei nodi del grafo*/

struct ListNode;

enum TypeNode {
    Var, App, Lam
};
enum Bool {
    True, False
};

/*********************************************************************************/
struct NodeVar {
    struct Node *binder;//NodeLam
};
struct NodeApp {
    struct Node *left;
    struct Node *right;
};
struct NodeLam {
    struct Node *left;//NodeVar
    struct Node *right;
};
struct Node {
    enum TypeNode label;
    union {
        struct NodeVar *var;
        struct NodeApp *app;
        struct NodeLam *lam;
    } content;
    struct Node *canonic;
    struct ListNode *parentNodes;
    struct ListNode *neighbour;
    enum Bool building;
};
/*********************************************************************************/

struct ListNode {
    struct ListNode *head;
    struct Node *node;
    struct ListNode *next;
    int count;
};

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

struct Node* InitVar() {
    struct Node* node= malloc(sizeof(struct Node));
    node->label= Var;
    struct NodeVar* var= malloc(sizeof(struct NodeVar));
    var->binder= NULL;
    node->content.var= var;
    node->canonic= NULL;
    node->building= False;
    node->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->parentNodes, node->parentNodes);
    node->neighbour = malloc(sizeof(struct ListNode));
    InitListNode(node->neighbour, node->neighbour);
    return node;
}

struct Node* InitApp(struct Node *left, struct Node *right) {
    struct Node* node= malloc(sizeof(struct Node));
    node->label= App;
    struct NodeApp* app= malloc(sizeof(struct NodeApp));
    app->left= left;
    app->right= right;
    node->content.app= app;
    node->canonic= NULL;
    node->building= False;
    node->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->parentNodes, node->parentNodes);
    node->neighbour = malloc(sizeof(struct ListNode));
    InitListNode(node->neighbour, node->neighbour);
    left->parentNodes = PushToListNode(left->parentNodes, node);
    right->parentNodes = PushToListNode(right->parentNodes, node);
    return node;
}

struct Node* InitLam(struct Node *left, struct Node *right) {
    if (left->label != Var) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un Var\n");
        exit(1);
    }
    struct Node* node= malloc(sizeof(struct Node));
    node->label= Lam;
    struct NodeLam* lam= malloc(sizeof(struct NodeLam));
    lam->left= left;
    lam->right= right;
    node->content.lam= lam;
    node->canonic= NULL;
    node->building= False;
    node->parentNodes = malloc(sizeof(struct ListNode));
    InitListNode(node->parentNodes, node->parentNodes);
    node->neighbour = malloc(sizeof(struct ListNode));
    InitListNode(node->neighbour, node->neighbour);
    left->content.var->binder = node;
    left->parentNodes = PushToListNode(left->parentNodes, node);
    right->parentNodes = PushToListNode(right->parentNodes, node);
    return node;
}

/********************************************************************************************************/

/**
 * Funzione utilizzata a scopo di test
 * stampa una lista di nodi specificando il tipo dei nodi ed eventualmente il tipo dei figli
 * nel caso Lam anche il tipo del binder del figlio sinistro
 * @param listNode

*/
void PrintListNode(struct ListNode *listNode) {
    printf("PrintListNode\n");
    int count =listNode->count;
    printf("Numero nodi %d\n", count);
    struct ListNode *nodes = listNode->head;
//    printf("PrintListNode - Nodes\n");
//    for (int i = 0; i < count; ++i) {
//        switch (nodes->node->label){
//            case Var:
//                printf("0\n");
//                break;
//            case App:
//                printf("1\n");
//                break;
//            case Lam:
//                printf("2\n");
//        }
//        nodes = nodes->next;
//    }
//    nodes = listNode->head;
    for (int i = 0; i < count; ++i) {
        printf("PrintListNode - Nodes\n");
        switch (nodes->node->label){
            case Var:
                printf("VarNode -Type Var\n");
                break;
            case App:
                printf("AppNode -Type App\n");
                printf("CRight -Type %d\n", nodes->node->content.app->right->label);
                printf("CLeft -Type %d\n", nodes->node->content.app->left->label);
                break;
            case Lam:
                printf("LamNode -Type Lam\n");
                printf("CRight -Type %d\n", nodes->node->content.lam->right->label);
                printf("CLeft -Type %d\n", nodes->node->content.lam->left->label);
                struct Node *temp = nodes->node->content.lam->left->content.var->binder;
                printf("binder -Type %d\n", temp->label);
                printf("\n\n");
        }
        nodes = nodes->next;
    }
}


/***************************************************************************************************/

/**
 * mette a confronto i 2 nodi passati in input ed esegue delle procedure ad hoc
 * tali procedure determinano se l'algoritmo termina fallendo
 * oppure creano gli archi indiretti necessari per il confronto dei nodi figli
 * @param m puntatore ad un nodo il quale deve essere messo a confronto con c
 * @param c puntatore ad un nodo il quale deve essere messo a confronto con m
 * @param queue del nodo m
 */
struct ListNode* EnqueueAndPropagate(struct Node *m, struct Node *c, struct ListNode *queue ) {
    switch ( m->label){
        case Var:
            if(c->label==Var){
                if(m!=c && (m->content.var->binder ==NULL || m->content.var->binder!=c->content.var->binder))
                    exit(1);
            } else
                exit(1);
            break;
        case App:
            if(c->label==App) {
                m->content.app->left->neighbour = PushToListNode(m->content.app->left->neighbour, c->content.app->left);
                m->content.app->right->neighbour = PushToListNode(m->content.app->right->neighbour, c->content.app->right);
                c->content.app->left->neighbour = PushToListNode(c->content.app->left->neighbour, m->content.app->left);
                c->content.app->right->neighbour = PushToListNode(c->content.app->right->neighbour, m->content.app->right);
            }
            else
                exit(1);
            break;
        case Lam:
            if(c->label==Lam){
                m->neighbour = PushToListNode(m->neighbour, c);
                c->neighbour = PushToListNode(c->neighbour, m);
            }else
                exit(1);
            break;
        default:
            exit(1);
    }
    m->canonic=c;
    PushToListNode(queue, m);
}

/**
 * fornito un nodo in input la funzione si richiama ricorsivamente sui genitori del nodo ed elabora gli archi indiretti
 * @param c
 */
void BuildEquivalenceClass(struct Node *c) {
    printf("BuildEquivalenceClass Node, Label= %d\n", c->label);
    c->canonic = c;
    c->building= True;
    struct ListNode *queue = malloc(sizeof(struct ListNode));
    InitListNode(queue, queue);
    queue = PushToListNode(queue, c);

    struct ListNode *headQueue = queue->head;
    int count=0;
    while (queue->count>count) {
        struct Node* n= headQueue->node;
        if (n->parentNodes != NULL) {
            struct ListNode *parents = n->parentNodes->head;
            for (int i = 0; i < n->parentNodes->count; ++i) {
                if (parents->node->canonic == NULL)
                    BuildEquivalenceClass(parents->node);
                else if (parents->node->canonic->building==True)
                    exit(1);
                parents = parents->next;
            }
        }
        if (n->neighbour != NULL) {
            struct ListNode *neighbour = n->neighbour->head;
            for (int i = 0; i < n->neighbour->count; ++i) {
                if (neighbour->node->canonic == NULL)
                    queue=  EnqueueAndPropagate(neighbour->node, c, queue);
                else if (neighbour->node->canonic!= c)
                    exit(1);
                neighbour = neighbour->next;
            }
        }
        count++;
        headQueue = headQueue->next;// queue(c).pop()
    }
    c->building= False;
}

/**
 * procedura che scorre tutti i nodi di una lista e
 * se hanno il canonic settato a NULL li visita tramite la procedura BuildEquivalenceClass
 * @param listNode lista di nodi da visitare
*/
void DAGCheck(struct ListNode *listNode) {
    struct ListNode *nodes = listNode->head;
    for (int i = 0; i < listNode->count; ++i) {
        if (nodes->node->canonic == NULL)
            BuildEquivalenceClass(nodes->node);
        nodes = nodes->next;
    }
}

/***************************************************************************************************/
/**
 * Test
 * @return 0 or error
 */
int main() {
    printf("Start Test\n");
    struct ListNode *nodes = malloc(sizeof(struct ListNode));
    InitListNode(nodes, nodes);

    struct Node* node1Var=InitVar();
    struct Node* node2Var=InitVar();
    struct Node* node1App=InitApp(node1Var, node2Var);
    struct Node* node3Var=InitVar();
    struct Node* node1Lam=InitLam(node3Var, node1App);

    nodes = PushToListNode(nodes, node1Var);
    nodes = PushToListNode(nodes, node2Var);
    nodes = PushToListNode(nodes, node1App);
    nodes = PushToListNode(nodes, node3Var);
    nodes = PushToListNode(nodes, node1Lam);

    struct Node* node11App=InitApp(node1Var, node2Var);
    struct Node* node31Var=InitVar();
    struct Node* node11Lam=InitLam(node31Var, node11App);

    nodes = PushToListNode(nodes, node11App);
    nodes = PushToListNode(nodes, node31Var);
    nodes = PushToListNode(nodes, node11Lam);

    PrintListNode(nodes);
    printf("\nStart Test PARTE2\n\n");

    DAGCheck(nodes);

    printf("END, World!\n");
    return 0;
}