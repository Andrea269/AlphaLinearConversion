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
    Var, Shared, App, Lam, Match, Let, FRic, GCoRic, Constructor, Constant
};
enum Bool {
    True, False
};

struct NodeVar {
    struct Node *binder;//NodeLam
};
struct NodeShared {
    struct Node *body;
};
struct NodeApp {
    struct Node *left;
    struct Node *right;
};
struct NodeLam {
    struct Node *var;//NodeVar
    struct Node *body;
};

struct NodeType {
    int dummy;
};
struct NodeJthConstr {
    int j;
    struct ListHT *arg;
};
struct NodeFRic {
    struct NodeType *f;
    int n;
    struct ListHT *arg;
};
struct NodeGCoRic {
    struct NodeType *g;
    int n;
    struct ListHT *arg;
};
struct NodeMatch {
    struct NodeType *i; //tipo su cui faccio match
    struct NodeType *t0;// il tipo dei rami
    struct Node *body;
    struct ListHT *branches; //array dei rami la cui lunghezza dipende dal tipo
};
struct NodeLet {
    struct Node *var;//NodeVar
    struct NodeType *t1;
    struct Node *t2;
    struct Node *t3;
};
struct NodeConst {
    struct Node *var;
};

struct Node {
    enum TypeNode label;
    union {
        struct NodeVar var;
        struct NodeShared shared;
        struct NodeApp app;
        struct NodeLam lam;
        struct NodeMatch match;
        struct NodeFRic fRic;
        struct NodeGCoRic gCoRic;
        struct NodeLet let;
        struct NodeJthConstr jCostr;
        struct NodeConst constant;//todo
    } content;
    struct Node *canonic;
    struct Node *copy;
    enum Bool building;
    enum Bool iswnf;
    struct ListHT *parentNodes;
    struct ListHT *neighbour;
    struct ListHT *queue;
};
struct Element {
    struct Node *node;
};
struct ListElement {
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

struct ListHT *InitListHT() {
    struct ListHT *list = malloc(sizeof(struct ListHT));
    struct ListElement *elementList = malloc(sizeof(struct ListElement));
    list->head = elementList;
    list->tail = elementList;
    list->count = 0;
    return list;
};

void PushToListHT(struct ListHT *list, struct Node *prtNode) {
    list->tail->node = prtNode;
    struct ListElement *elementList = malloc(sizeof(struct ListElement));
    list->tail->next = elementList;
    list->tail = elementList;// list->tail->next
    list->count = list->count + 1;
};

void PushParentListHT(struct Node *node, struct ListHT *list) {
    struct ListElement *listElement;
    listElement = list->head;
    for (int i = 0; i < list->count; ++i) {
        PushToListHT(listElement->node->parentNodes, node);
        listElement = listElement->next;
    }
}

struct Node *InitVar(struct Node *binder) {
    if (binder != NULL && binder->label != Lam) {
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    return node;
}

struct Node *InitShared(struct Node *body) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Shared;
    node->content.shared.body = body;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
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
    node->parentNodes = InitListHT();
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
//    PushToListHT(var->content.var.binder, node);
    var->content.var.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    return node;
}

struct Node *InitConstructor(int j, struct ListHT *arg) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Constructor;
    node->content.jCostr.j = j;
    node->content.jCostr.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    return node;
}

struct Node *InitFRic(struct NodeType *f, int n, struct ListHT *arg) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = FRic;
    node->content.fRic.f = f;
    node->content.fRic.n = n;
    node->content.fRic.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    return node;
}

struct Node *InitGCoRic(struct NodeType *g, int n, struct ListHT *arg) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = GCoRic;
    node->content.gCoRic.g = g;
    node->content.gCoRic.n = n;
    node->content.gCoRic.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    return node;
}

struct Node *InitMatch(struct NodeType *i, struct NodeType *t0, struct Node *body, struct ListHT *branches) {
    if (!(body->label == Constructor || body->label == GCoRic)) {
        printf("Exit InitMatchConstructor-> error in un nodo Match il nodo body deve essere un Constructor o un GCoRic \n");
        exit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Match;
    node->content.match.i = i;
    node->content.match.t0 = t0;
    node->content.match.body = body;
    node->content.match.branches = branches;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, branches);
    PushToListHT(body->parentNodes, node);
    return node;
}

struct Node *InitLet(struct Node *var, struct NodeType *t1, struct Node *t2, struct Node *t3) {
    if (var->label != Var) {
        printf("Exit InitLet-> error in un nodo Let il nodo var deve essere un Var\n");
        exit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Let;
    node->content.let.var = var;
    node->content.let.t1 = t1;
    node->content.let.t2 = t2;
    node->content.let.t3 = t3;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->iswnf = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.var.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(t2->parentNodes, node);
    PushToListHT(t3->parentNodes, node);
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
            else if (l->label == Lam && src == l->content.lam.var)
                dst = Unchain(sub);// Unchain qui anzichè nella chiamata così viene eseguito solo se usato il nodo sub
            else if (src->content.var.binder->copy == NULL)
                dst = src;
            else
                dst = src->content.var.binder->copy;
            break;
        case App:
            dst = InitApp(Subst(src->content.app.left, NULL, l, sub), Subst(src->content.app.right, NULL, l, sub));
            break;
        case Lam:
            src->copy = dst;
            src->content.lam.body = Subst(src->content.lam.body, NULL, l, sub);
            src->copy = NULL;
            dst = src;
            break;
        case Match:
            break;
        case Let:
            break;
        case FRic:
            break;
        case GCoRic:
            break;
        case Constructor:
            break;
        case Constant:
            break;
    }
    return dst;
}

struct Node *ReductionJ(struct Node *tj, struct ListHT *args) {
    struct Node *result = tj;
    struct ListElement *arg = args->head;
    for (int i = 0; i < args->count; ++i) {
        result = InitApp(result, arg->node);
        arg = arg->next;
    }
    return result;
}

void Eval(struct Element *element) {//riduzione....
    struct Node *n = element->node;
    struct Node *b = InitVar(NULL);//todo temp for test
    struct ListElement *listElement;
    struct Element *e = malloc(sizeof(struct Element));
    printf("Eval Node, Label= %d\n", n->label);
    if (n->iswnf == True)
        return;
    switch (n->label) {
        case Var:
        case Shared:
        case Lam:
        case GCoRic:
            n->iswnf = True;
            break;
        case App://beta todo ... dove creo il collegamento al nodo destro del nodo App???????
            e->node = n->content.app.right;
            Eval(e);
            e->node = n->content.app.left;
            Eval(e);
            switch (n->content.app.left->label) {
                case Lam:
                    element->node = Subst(n->content.app.left->content.lam.body, n, n->content.app.left,
                                          n->content.app.right);
                    Eval(element);
                    break;
                case Shared:
                    if (n->content.app.left->content.shared.body->label == Lam) {
                        element->node = Subst(n->content.app.left->content.shared.body->content.lam.body,
                                              n, n->content.app.left, n->content.app.right);
                        Eval(element);
                        break;
                    }
                default:
                    n->iswnf = True;
            }
            break;
        case Match:
            switch (n->content.match.body->label) {
                case Constructor://j
                    listElement = n->content.match.branches->head;
                    for (int i = 0; i < n->content.match.body->content.jCostr.j - 1; ++i) {
                        listElement = listElement->next;
                    }
                    e->node = listElement->node;
                    Eval(e);//valuto il ramo j-esimo e poi applico la j
                    element->node = ReductionJ(e->node, n->content.match.body->content.jCostr.arg);
                    Eval(element);
                    break;
                case GCoRic://jDelta-c
                    n->content.match.body = ReductionJ(b, n->content.match.body->content.gCoRic.arg);
                    Eval(element);
                    break;
                default:
                    break;
            }
            break;
        case Let://todo testare
            e->node = n->content.let.t2;
            Eval(e);
            n->content.let.var->content.var.binder = n->content.let.t2;
            element->node = n->content.let.t3;
            Eval(element);
            break;
        case FRic://jDelta-i todo testare
            element->node = ReductionJ(b, n->content.fRic.arg);
            break;
        case Constructor:
            break;
        case Constant://Delta
            break;
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

void Propagate(struct Node *m, struct Node *c) {//propagazione nodi....
    switch (m->label) {
        case Shared:
        case Let:
        case FRic:
        case GCoRic:
        case Constructor:
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
            break;
        case Match:
            if (c->label == Match)
                PushNeighbour(m->content.match.body, c->content.match.body);
            else
                exit(3);
            break;
        case Constant:
            if (c->label != Constant)
                exit(3);
            break;
    }
}

void nodeRefactoring(struct Node *n, struct Node *e) {//conserva il puntatore originale
    n->label = e->label;
    n->content = e->content;
    struct ListElement *parent = e->parentNodes->head;
    for (int i = 0; i < e->parentNodes->count; ++i) {//forse inutile ricopiare i parent poichè già analizzati
        PushToListHT(n->parentNodes, parent->node);
        parent = parent->next;
    }
}

void BuildClass(struct Node *c) {
    printf("BuildClass Node, Label= %d\n", c->label);
    c->building = True;
    c->queue = InitListHT();
    struct ListElement *iterQueue = c->queue->head;
    Enqueue(c, c);
    struct Element *e = malloc(sizeof(struct Element));
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
        e->node = n;
        Eval(e);
        if (n != e->node)//mantenendo lo stesso puntatore di n ma aggiornando i parent e il content (e di conseguenza anche la label)
            nodeRefactoring(n, e->node);

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
            for (int i = 0; i < n->parentNodes->count; ++i) {//itero sui padri di n e se il padre è shared valuto
                if (iterParents->node->label == Shared) {
                    if (iterParents->node->canonic == NULL)
                        Enqueue(iterParents->node, c);
                    else if (iterParents->node->canonic != c)
                        exit(2);
                    iterParents = iterParents->next;
                }
            }
            if (n->label == Shared) {//se n è shared valuto sul body
                struct Node *body = n->content.shared.body;
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

void initTest1() {
    struct Node *node1Var = InitVar(NULL);
    struct Node *node2Var = InitVar(NULL);
    struct Node *node1App = InitApp(node1Var, node2Var);
    struct Node *node3Var = InitVar(NULL);
    struct Node *node1Lam = InitLam(node3Var, node1App);
    struct Node *node11App = InitApp(node1Var, node2Var); //test build correct
//    struct Node *node11App = InitApp(node1Var, node3Var); //test build error
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
}

void initTest2() {

    struct Node *node1Var = InitVar(NULL);
    struct Node *node2Var = InitVar(NULL);
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);//figlio dex body lam


    struct Node *node2Lam = InitLam(node1Var, node2Var);
    struct Node *node2App = InitApp(node2Lam, node2Var);


    nodesHT = InitListHT();
    PushToListHT(nodesHT, node1Var);
    PushToListHT(nodesHT, node2Var);
    PushToListHT(nodesHT, node1Lam);
    PushToListHT(nodesHT, node1App);
    //inutile?
    PushToListHT(nodesHT, node2Lam);
    PushToListHT(nodesHT, node2App);

    PushNeighbour(node1App, node2App);
}

void initTestMatch() {
    struct NodeType *type = malloc(sizeof(struct NodeType));

    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitVar(NULL);
    struct Node *node12Var = InitVar(NULL);
    struct Node *node13Var = InitVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitVar(NULL);
    struct Node *node2Var = InitVar(NULL);
    struct Node *node3Var = InitVar(NULL);

    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node1Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);


    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam, node2Var);
    struct Node *node2App2 = InitApp(node1Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);

    struct Node *nodeConst = InitConstructor(3, arg);
    struct Node *nodeMatchConst = InitMatch(type, type, nodeConst, branches);
//    struct Node *nodeGcoric=InitGCoRic(type, 3, arg);
//    struct Node *nodeMatchGcoric = InitMatch( type, type, nodeGcoric, branches);
    struct Node *nodeConst2 = InitConstructor(3, arg);
    struct Node *nodeMatchConst2 = InitMatch(type, type, nodeConst2, branches2);
//    struct Node *nodeGcoric2=InitGCoRic(type, 3, arg);
//    struct Node *nodeMatchGcoric2 = InitMatch( type, type, nodeGcoric2, branches2);

    nodesHT = InitListHT();
    PushToListHT(nodesHT, node11Var);
    PushToListHT(nodesHT, node12Var);
    PushToListHT(nodesHT, node13Var);
    PushToListHT(nodesHT, node1Var);
    PushToListHT(nodesHT, node2Var);
    PushToListHT(nodesHT, node3Var);
    PushToListHT(nodesHT, node1Lam);
    PushToListHT(nodesHT, node1App);
    PushToListHT(nodesHT, node2App);
    PushToListHT(nodesHT, node3App);
    PushToListHT(nodesHT, nodeConst);
    PushToListHT(nodesHT, nodeMatchConst);
//    PushToListHT(nodesHT, nodeGcoric);
//    PushToListHT(nodesHT, nodeMatchGcoric);

    //inutile?
//    PushToListHT(nodesHT, node1Lam2);
//    PushToListHT(nodesHT, node1App2);
//    PushToListHT(nodesHT, node2App2);
//    PushToListHT(nodesHT, node3App2);
//    PushToListHT(nodesHT, nodeConst2);
//    PushToListHT(nodesHT, nodeMatchConst2);
//    PushToListHT(nodesHT, nodeGcoric2);
//    PushToListHT(nodesHT, nodeMatchGcoric2);

    PushNeighbour(nodeMatchConst, nodeMatchConst2);
//    PushNeighbour(nodeMatchGcoric, nodeMatchGcoric2);
//    PushNeighbour(nodeMatchConst, nodeMatchGcoric2);
//    PushNeighbour(nodeMatchGcoric, nodeMatchConst2);
}



/**
 * @return  0-> Successfully completed
 *          1-> ERROR: Exit in InitNode
 *          2-> ERROR: Exit in BuildClass
 *          3-> ERROR: Exit in Propagate
*/
int main() {
    printf("START Test\n\n");

//    initTest1();
//    initTest2();
    initTestMatch(); // exit 3-> if (n->label == Shared) {//se n è shared valuto sul body


    PrintListHT();
    DAGCheckAndEval();

//    printf("\n\n");
//    PrintListHT();
    printf("\nEND Test\n");
    return 0;
}
/********************************************************************************************************/