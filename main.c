//
// Created by andrea on 13/05/19.
//
#include <stdio.h>
#include <stdlib.h>

/***********************************************************************************************
Strutture dati necessarie alla memorizzazione dei dati nei nodi del grafo*/

struct ListHT;
struct ListHT *nodesHT;

enum TypeNode {
    Var, Shared, App, Lam
};
enum Bool {
    True, False
};

struct NodeVar {
    struct Node *binder;//NodeLam
};
struct NodeShared {
    struct Node *body;//NodeVar
};
struct NodeApp {
    struct Node *left;
    struct Node *right;
};
struct NodeLam {
    struct Node *var;//NodeVar
    struct Node *body;
};
struct Node {
    enum TypeNode label;
    union {
        struct NodeVar var;
        struct NodeShared shared;
        struct NodeApp app;
        struct NodeLam lam;
    } content;
    struct Node *canonic;
    struct Node *copy;
    enum Bool building;
    enum Bool iswnf;
    struct ListHT *parentNodes;
    struct ListHT *neighbour;
    struct ListHT *queue;
};
struct ListElement{
    struct Node *node;
    struct ListElement *next;
};
struct ListHT {
    struct ListElement *head;
    struct ListElement *tail;
    int count;
};
/*********************************************************************************
 Funzioni utili*/

struct ListHT* InitListHT() {
    struct ListHT* list= malloc(sizeof(struct ListHT));
    struct ListElement* elementList= malloc(sizeof(struct ListElement));
    list->head=elementList;
    list->tail=elementList;
    list->count = 0;
    return list;
};
void PushToListHT(struct ListHT *list, struct Node *prtNode) {
    list->tail->node = prtNode;
    struct ListElement* elementList= malloc(sizeof(struct ListElement));
    list->tail->next=elementList;
    list->tail=elementList;// list->tail->next
    list->count=list->count+1;
};

struct Node *InitVar(struct Node *binder) {
    if (binder!=NULL && binder->label!= Lam) {
        printf("Exit InitVar-> error in un nodo Var il nodo binder deve essere un Lam\n");
        exit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Var;
    node->content.var.binder = binder;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes =InitListHT();
    node->neighbour = InitListHT();
    return node;
}

struct Node *InitShared(struct Node *body) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Shared;
    node->content.shared.body= body;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes =InitListHT();
    node->neighbour = InitListHT();
    return node;
}

struct Node *InitApp(struct Node *left, struct Node *right) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = App;
    node->content.app.left = left;
    node->content.app.right = right;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes =InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(left->parentNodes, node);
    PushToListHT(right->parentNodes, node);
    return node;
}

struct Node *InitLam(struct Node *var, struct Node *body) {
    if (var->label != Var) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un Var\n");
        exit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Lam;
    node->content.lam.var = var;
    node->content.lam.body = body;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes =InitListHT();
    node->neighbour = InitListHT();
    var->content.var.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    return node;
}
/***************************************************************************************************
 Evaluate*/

struct Node *Unchain(struct Node *n) {
    if (n->label == Shared)
        return n;
    else {
        struct Node *sharedN = InitShared(n);
        PushToListHT(n->parentNodes, sharedN);
        return sharedN;
    }
}

struct Node *Subst(struct Node *src, struct Node *dst, struct Node *l, struct Node *sub) {
    if (dst == NULL) {
        dst = malloc(sizeof(struct Node));
        PushToListHT(nodesHT, dst);
    }

    switch (src->label) {
        case Shared:
            dst = src;
            break;
        case Var:
            if (src->content.var.binder == NULL)
                dst = src;
            else if (src->content.var.binder == l)
                dst = Unchain(sub);
            else if (src->content.var.binder->copy == NULL)
                dst = src;
            else
                dst = src->content.var.binder->copy;
            break;
        case App:
            dst= InitApp(Subst(src->content.app.left, NULL, l, sub), Subst(src->content.app.right, NULL, l, sub));
            break;
        case Lam:
            src->copy=dst;
            src->content.lam.body=Subst(src->content.lam.body, NULL, l, sub);
            src->copy=NULL;
            dst=src;
            break;
    }
    return dst;
}

void Eval(struct Node *n) {
    if (n->iswnf == True)
        return;
    switch (n->label) {
        case Lam:
        case Var:
        case Shared:
            n->iswnf = True;
            break;
        case App:
            Eval(n->content.app.right);
            Eval(n->content.app.left);
            switch (n->content.app.left->label) {
                case Lam:
                    Subst(n->content.app.left->content.lam.body, n, n->content.app.left, n->content.app.right);
                    Eval(n);
                    break;
                case Shared:
                    if (n->content.app.left->content.shared.body->label == Lam) {
                        break;
                    }
                default:
                    n->iswnf = True;
            }
    }
}

/***************************************************************************************************
 BuildEquivalenceClass*/
void PushNeighbour(struct Node *m, struct Node *c) {
    PushToListHT(m->neighbour, c);
    PushToListHT(c->neighbour, m);
}

void Enqueue(struct Node *n, struct Node *c) {
    n->canonic = c;
    PushToListHT(c->queue, n);
}

void Propagate(struct Node *m, struct Node *c) {
    switch (m->label) {
        case Shared:
            break;
        case Var:
            if (c->label == Var) {
                if (m->content.var.binder == NULL || c->content.var.binder == NULL ||
                               m->content.var.binder->canonic != c->content.var.binder->canonic)
                    exit(3);
            } else
                exit(3);
            break;
        case App:
            if (c->label == App) {
                PushNeighbour(m->content.app.left, c->content.app.left);
                PushNeighbour(m->content.app.right, c->content.app.right);
            } else
                exit(3);
            break;
        case Lam:
            if (c->label == Lam)
                PushNeighbour(m->content.lam.body, c->content.lam.body);
            else
                exit(3);
    }
}

void BuildClass(struct Node *c) {
    printf("BuildClass Node, Label= %d\n", c->label);
    c->building = True;
    c->queue= InitListHT();
    struct ListElement *iterQueue = c->queue->head;
    Enqueue(c, c);
    int count = 0;
    while (c->queue->count > count) {
        struct Node *n = iterQueue->node;
        // visit parents
        if (n->parentNodes != NULL) {
            struct ListElement *iterParents = n->parentNodes->head;
            for (int i = 0; i < n->parentNodes->count; ++i) {
                if (iterParents->node->label == Shared)// m=iterParents->node
                    continue;

                if (iterParents->node->canonic == NULL)
                    BuildClass(iterParents->node);
                else if (iterParents->node->canonic->building == True)
                    exit(2);
                iterParents = iterParents->next;
            }
        }
        //garbage collect
        /** if n has no parents and no ∼neighbours then  gc(n); return */
        // evaluate
        Eval(n);
        // propagate
        if (n != c)
            Propagate(n, c);
        // visit neighbours
        if (n->neighbour != NULL) {
            struct ListElement *iterNeighbour = n->neighbour->head;
            for (int i = 0; i < n->neighbour->count; ++i) {
                if (iterNeighbour->node->canonic == NULL)
                    Enqueue(iterNeighbour->node, c);
                else if (iterNeighbour->node->canonic != c)
                    exit(2);
                iterNeighbour = iterNeighbour->next;
            }
            struct ListElement *iterParents = n->parentNodes->head;
            for (int i = 0; i < n->neighbour->count; ++i) {//itero sui padri di n e se il padre è shared valuto
                if(iterParents->node->label==Shared){
                    if (iterParents->node->canonic == NULL)
                        Enqueue(iterParents->node, c);
                    else if (iterParents->node->canonic != c)
                        exit(2);
                    iterParents = iterParents->next;
                }
            }
            if(n->label==Shared){//se n è shared valuto sul body
                struct Node* body=n->content.shared.body;
                if (body->canonic == NULL)
                    Enqueue(body, c);
                else if (body->canonic != c)
                    exit(2);
            }
        }
        count++;
        iterQueue = iterQueue->next;
    }
    c->building = False;
}

/***************************************************************************************************
 Main-Equivalence&Eval*/
void DAGCheckAndEval() {//struct ListHT *listHT
    printf("START DAGCheckAndEval\n");
    struct ListElement *nodes = nodesHT->head;
    for (int i = 0; i < nodesHT->count; ++i) {
        if (nodes->node->canonic == NULL && nodes->node->label != Shared)
            BuildClass(nodes->node);
        nodes = nodes->next;
    }
    printf("END DAGCheckAndEval\n");
}

/**************************************************************************************************
 TEST*/
void PrintListHT() {//struct ListHT *listHT
    printf("START PrintListHT\n");
    int count = nodesHT->count;
    printf("Numero nodi %d\n", count);
    struct ListElement *nodes = nodesHT->head;

    for (int i = 0; i < count; ++i) {
        printf("PrintListNode - Nodes\n");
        switch (nodes->node->label) {
            case Var:
                printf("VarNode -Type Var\n");
                break;
            case Shared:
                printf("VarNode -Type Shared\n");
                break;
            case App:
                printf("AppNode -Type App\n");
                printf("CRight -Type %d\n", nodes->node->content.app.right->label);
                printf("CLeft -Type %d\n", nodes->node->content.app.left->label);
                break;
            case Lam:
                printf("LamNode -Type Lam\n");
                printf("CRight -Type %d\n", nodes->node->content.lam.body->label);
                printf("CLeft -Type %d\n", nodes->node->content.lam.var->label);
                struct Node *temp = nodes->node->content.lam.var->content.var.binder;
                printf("binder -Type %d\n", temp->label);
        }
        nodes = nodes->next;
    }
    printf("END PrintListHT\n\n");
}
/**
 * @return  0-> Successfully completed
 *          1-> ERROR: Exit in InitNode
 *          2-> ERROR: Exit in BuildClass
 *          3-> ERROR: Exit in Propagate
*/
int main() {
    printf("START Test\n\n");
    struct Node *node1Var = InitVar(NULL);
    struct Node *node2Var = InitVar(NULL);
    struct Node *node1App = InitApp(node1Var, node2Var);
    struct Node *node3Var = InitVar(NULL);
    struct Node *node1Lam = InitLam(node3Var, node1App);
    struct Node *node11App = InitApp(node1Var, node2Var);
    struct Node *node11Lam = InitLam(node3Var, node11App);

    nodesHT = InitListHT();
    PushToListHT(nodesHT, node1Var);
    PushToListHT(nodesHT, node2Var);
    PushToListHT(nodesHT, node1App);
    PushToListHT(nodesHT, node3Var);
    PushToListHT(nodesHT, node1Lam);
    //inutile?
    PushToListHT(nodesHT, node11App);
    PushToListHT(nodesHT, node11Lam);

    PushNeighbour(node1Lam, node11Lam);

    PrintListHT();
    DAGCheckAndEval();

    printf("\nEND Test\n");
    return 0;
}
/********************************************************************************************************/