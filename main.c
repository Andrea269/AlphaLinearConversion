
#include <stdio.h>
#include <stdlib.h>

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
    struct IndirectEdge edgePhysical;
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
//struct Node InitDeadNode() {
//    struct Node node;
//    node.isAlive = 0;
//    return node;
//}

void InitListNode(struct ListNode *listNode, struct ListNode *head) {
    listNode->head = head;
    listNode->node = malloc(sizeof(struct Node));
    listNode->next = malloc(sizeof(struct ListNode));
    listNode->count = 0;
};

struct IndirectEdge InitIndirectEdge() {
    struct IndirectEdge indirectEdge;
    indirectEdge.left = NULL;
    indirectEdge.right = NULL;
    indirectEdge.isAlive = 0;
    return indirectEdge;
}

void InitListIndirectEdge(struct ListIndirectEdge *listNode, struct ListIndirectEdge *head) {
    listNode->head = head;
    listNode->edge = malloc(sizeof(struct IndirectEdge));//InitIndirectEdge();
    listNode->edgePhysical = InitIndirectEdge();
    listNode->next = malloc(sizeof(struct ListIndirectEdge));
    listNode->count = 0;
};

struct ListNode *PushToListNode(struct ListNode *list, struct Node *prtNode) {
    list->node = prtNode;
    InitListNode(list->next, list->head);
    list->next->count = list->count + 1;
    return list->next;
};

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

void InitLam(struct Node *node, struct Node *left, struct Node *right) {
    if (left->type != Var) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un Var\n");
        exit(0);
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
struct ListIndirectEdge *PushToListIndirectEdge(struct ListIndirectEdge *list, struct Node *left, struct Node *right) {
    list->edgePhysical.isAlive = 1;
    list->edgePhysical.left = left;
    list->edgePhysical.right = right;
    list->edge = &list->edgePhysical;
    InitListIndirectEdge(list->next, list->head);
    list->next->count = list->count + 1;
    return list->next;
};


void CreateIndirectEdge(struct Node *left, struct Node *right) {
    left->indirectEdge = PushToListIndirectEdge(left->indirectEdge, left, right);
    right->indirectEdge = PushToListIndirectEdge(right->indirectEdge, left, right);
}

struct Node *FindOtherEndEdge(struct IndirectEdge *indirectEdge, struct Node *source) {
    if (source == indirectEdge->left)
        return indirectEdge->right;
    else if (source == indirectEdge->right)
        return indirectEdge->left;
    return NULL;//"Error! Impossible to find the target";
}

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
struct ListNode *Push(struct ListNode *queue, struct Node *d, struct Node *t, struct IndirectEdge *edge) {
    if (t->canonic == NULL) {
        t->canonic = d;
        edge->isAlive = 0;
        return PushToListNode(queue, t);
    } else if (t->canonic != d) {
        printf("Exit ramo Push->t.canonic != d \n");
        exit(0);//error
    }
}

void Propagate(struct Node *d, struct Node *h) {
    if (d->type == Var && h->type == Var) {
        if (d->binder == NULL || h->binder == NULL) {//todo ????
            printf("Exit ramo Propagate->d->binder == NULL || h->binder == NULL\n");
//todo            if(i!=j)????
            exit(0);

        } else if (d->binder->canonic != h->binder->canonic) {
            printf("Exit ramo Propagate->d->binder->canonic!=h->binder->canonic\n");
            exit(0);//different canonic
        }
    } else {
        printf("CreateIndirectEdge ramo Propagate \n");
        CreateIndirectEdge(d->relativeNode->cLeft, h->relativeNode->cLeft);
        CreateIndirectEdge(d->relativeNode->cRight, h->relativeNode->cRight);
    }
    h->isAlive = 0;
}

void Kill(struct Node *d) {
    printf("Kill Node Test, %d\n", d->type);
    struct ListNode *queue = malloc(sizeof(struct ListNode));
    InitListNode(queue, queue);

    if (d->canonic == NULL)
        d->canonic = d;
    else {
        printf("Exit ramo Kill->d->canonic == NULL---- Else\n");
        exit(0);//erroe
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
            exit(0);
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

void BlindSharingCheck(struct ListNode *listNode) {
    struct ListNode *nodes = listNode->head;
    for (int i = 0; i < listNode->count; ++i) {
        if (nodes->node->isAlive != 0)
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

    struct Node node11Var;
    InitVar(&node11Var);
    struct Node node21Var;
    InitVar(&node21Var);
    struct Node node11App;
    InitApp(&node11App, &node11Var, &node21Var);
    struct Node node31Var;
    InitVar(&node31Var);
    struct Node node11Lam;
    InitLam(&node11Lam, &node31Var, &node11App);

    nodes = PushToListNode(nodes, &node11Var);
    nodes = PushToListNode(nodes, &node21Var);
    nodes = PushToListNode(nodes, &node11App);
    nodes = PushToListNode(nodes, &node31Var);
    nodes = PushToListNode(nodes, &node11Lam);


//    struct Node root;
//    InitApp(&root, &node1Lam, &node11Lam);
//    nodes = PushToListNode(nodes, &root);
    CreateIndirectEdge(&node1Lam, &node11Lam);


    PrintListNode(nodes);
    printf("\nStart Test PARTE2\n\n");

    BlindSharingCheck(nodes);


    printf("END, World!\n");



//    PrintListNode(nodes);

//    printf("\nStart Test PARTE2\n\n");
//
//    CreateIndirectEdge(&node1Var, &node3Var);
//    struct Node *verfy = FindOtherEndEdge(&node1Var.indirectEdge->head->edge, &node1Var);
//    if (verfy == &node3Var)
//        printf("verfy==&node3Var True!\n");
//    struct Node *verfy2 = FindOtherEndEdge(&node1Var.indirectEdge->head->edge, &node2Var);
//    if (verfy2 != NULL)
//        printf("verfy2 False!\n");
//    else
//        printf("verfy2 True!\n");
//


    return 0;
}



//struct Graph{
//    struct ListNode *nodes;
//};
//void InitGraph(struct Graph *graph) {
//    graph->nodes = malloc(sizeof(struct ListNode));
//    InitListNode(graph->nodes, graph->nodes);
//};
//struct Node AddVar(struct Graph *graph){
//    struct Node nodeVar = InitVar();
//    graph->nodes= PushToListNode(graph->nodes, nodeVar);
//    return nodeVar;
//}
//struct Node AddApp(struct Graph *graph, struct Node left, struct Node right){
//    struct Node nodeApp = InitApp(&left, &right);
//    graph->nodes= PushToListNode(graph->nodes, nodeApp);
//    right.relativeNode->parentNodes=PushToListNode(right.relativeNode->parentNodes, nodeApp);
//    left.relativeNode->parentNodes=PushToListNode(left.relativeNode->parentNodes, nodeApp);
//    return nodeApp;
//}
//struct Node AddLam(struct Graph *graph, struct Node left, struct Node right){
//    struct Node nodeLam = InitLam(&left, &right);
//    graph->nodes= PushToListNode(graph->nodes, nodeLam);
//    right.relativeNode->parentNodes=PushToListNode(right.relativeNode->parentNodes, nodeLam);
//    left.relativeNode->parentNodes=PushToListNode(left.relativeNode->parentNodes, nodeLam);
//    left.binder = &nodeLam;
//    return nodeLam;
//}
//int main() {
//    printf("\nStart GRAPH\n\n");
//
//    struct Graph *graph=malloc(sizeof(struct Graph));
//    InitGraph(graph);
//    struct Node node1VarGraph=AddVar(graph);
//    struct Node node2VarGraph=AddVar(graph);
//    struct Node node1AppGraph=AddApp(graph, node1VarGraph, node2VarGraph);
//
//    struct Node node3VarGraph=AddVar(graph);
//    struct Node node1LamGraph=AddLam(graph, node3VarGraph, node1AppGraph);
//    PrintListNode(graph->nodes);
//
//    printf("END, World!\n");
//    return 0;
//}