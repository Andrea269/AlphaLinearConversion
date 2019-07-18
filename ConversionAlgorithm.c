//
// Created by andrea on 17/07/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/********---DATA STRUCTURES---*************/
struct ListHT;

enum TypeNode {
    FVar, BVar, Shared, App, Lam, Match, Let, FRic, GCoRic, Constructor, Constant
};
enum Bool {
    True, False
};

struct NodeFVar {
    struct Node *var;
};
struct NodeBVar {
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
    struct Node *var;//NodeBVar
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
    struct Node *var;//NodeBVar
    struct Node *t;
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
    struct ListHT *branches; //lista dei rami la cui lunghezza dipende dal tipo
};
struct NodeLet {
    struct Node *var;//NodeBVar
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
        struct NodeFVar fvar;
        struct NodeBVar bvar;
        struct NodeShared shared;
        struct NodeApp app;
        struct NodeLam lam;
        struct NodeMatch match;
        struct NodeFRic fRic;
        struct NodeGCoRic gCoRic;
        struct NodeLet let;
        struct NodeJthConstr jCostr;
        struct NodeConst constant;
    } content;
    struct Node *canonic;
    struct Node *copy;
    enum Bool building;
    struct ListHT *parentNodes;
    struct ListHT *neighbour;
    struct ListHT *queue;
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

/*****************************---USEFUL FUNCTIONS---************************************/
void PrintExit(int error) {
    printf("\nFail: Process finished with exit code %d\n", error);
    exit(error);
}
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

struct Node *InitFVar(struct Node *var) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = FVar;
    node->content.fvar.var = var;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    return node;
}
struct Node *InitBVar(struct Node *binder) {
    if (binder != NULL && binder->label != Lam) {
        printf("Exit InitVar-> error in un nodo Var il nodo binder deve essere un Lam\n");
        PrintExit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = BVar;
    node->content.bvar.binder = binder;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(left->parentNodes, node);
    PushToListHT(right->parentNodes, node);
    return node;
}
struct Node *InitLam(struct Node *var, struct Node *body) {
    if (var->label != BVar) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un BVar\n");
        PrintExit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Lam;
    node->content.lam.var = var;
    node->content.lam.body = body;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    return node;
}
struct Node *InitFRic(struct Node *var, struct Node *body, int n, struct ListHT *arg) {
    if (var->label != BVar) {
        printf("Exit InitLam-> error in un nodo FRic il nodo var deve essere un BVar\n");
        PrintExit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = FRic;
    node->content.fRic.var = var;
    node->content.fRic.t= body;
    node->content.fRic.n = n;
    node->content.fRic.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    return node;
}
struct Node *InitMatch(struct NodeType *i, struct NodeType *t0, struct Node *body, struct ListHT *branches) {
    if (!(body->label == Constructor || body->label == GCoRic)) {
        printf("Exit InitMatchConstructor-> error in un nodo Match il nodo body deve essere un Constructor o un GCoRic \n");
        PrintExit(1);
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, branches);
    PushToListHT(body->parentNodes, node);
    return node;
}
struct Node *InitLet(struct Node *var, struct NodeType *t1, struct Node *t2, struct Node *t3) {
    if (var->label != BVar) {
        printf("Exit InitLet-> error in un nodo Let il nodo var deve essere un Var\n");
        PrintExit(1);
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
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(t2->parentNodes, node);
    PushToListHT(t3->parentNodes, node);
    return node;
}
/*************************************---EVALUATE---*****************************************/
struct Node *Inst(struct Node *n, struct Node *l, struct Node *sub) {
    struct Node *n1;
    switch (n->label) {
        case FVar:
            return n;
        case BVar:
            if (n->content.bvar.binder == l)
                return sub;
            else {
                if (n->content.bvar.binder->copy == NULL)
                    return n;
                else
                    return InitBVar(n->content.bvar.binder->copy);
            }
        case Shared:
            return n;
        case App:
            return InitApp(Inst(n->content.app.left, l, sub), Inst(n->content.app.right, l, sub));
        case Lam:
            n1 = InitLam(InitBVar(NULL), InitFVar(NULL));
            n->copy = n1;
            n1->content.lam.body = Inst(n->content.lam.body, l, sub);
            n->copy = NULL;
            return n1;
        case Match:
            return n;
        case Let://todo
            break;
        case FRic://todo
            break;
        case GCoRic://todo
            break;
        case Constructor://todo
            break;
        case Constant://todo
            break;
    }
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

struct Node *WeakCbVEval(struct Node *n) {//riduzione....
    printf("Eval Node, Label= %d\n", n->label);
    struct ListElement *listElement;
    struct Node *n1;
    struct Node *n2;
    switch (n->label) {
        case FVar:
            return n;
        case BVar:
            return n;
        case Shared:
            return n->content.shared.body;
        case App:
            n2 = WeakCbVEval(n->content.app.right);
            n1 = WeakCbVEval(n->content.app.left);
            switch (n1->label) {
                case Lam:
                    return WeakCbVEval(Inst(n1->content.lam.body, n1, InitShared(n2)));
                default:
                    return InitApp(n1, n2);
            }
        case Lam:
            return n;
        case Match:
            switch (n->content.match.body->label) {
                case Constructor://j
                    listElement = n->content.match.branches->head;
                    for (int i = 0; i < n->content.match.body->content.jCostr.j - 1; ++i) {
                        listElement = listElement->next;
                    }
                    //valuto il ramo j-esimo e poi applico la j
                    n1=WeakCbVEval(listElement->node);
                    n2 = ReductionJ(n1, n->content.match.body->content.jCostr.arg);
                    return  WeakCbVEval(n2);
                case GCoRic://jDelta-c todo ????
                    n1 = InitFVar(NULL);//todo b tempVar for test
                    n->content.match.body = ReductionJ(n1, n->content.match.body->content.gCoRic.arg);
                    return WeakCbVEval(n);
                default:
                    return n;
            }
        case Let:
            n->content.let.var->content.bvar.binder = WeakCbVEval(n->content.let.t2);
            n1 = n->content.let.t3;
            return WeakCbVEval(n1);
        case FRic:
            listElement = n->content.fRic.arg->head;
            for (int i = 0; i < n->content.fRic.arg->count - 1; ++i) {
                listElement = listElement->next;
            }
            //valuto l'arg j-esimo
            listElement->node=WeakCbVEval(listElement->node);
            if(listElement->node->label==Constructor){
                n1=Inst(n->content.fRic.t, n->content.fRic.var, n);//b
                return WeakCbVEval(ReductionJ(n1, n->content.fRic.arg));
            }
            n->content.fRic.t=WeakCbVEval(n->content.fRic.t);
            return n;
        case GCoRic:
            return n;
        case Constructor:
            return n;
        case Constant:
            return n->content.constant.var;
    }
}
/***************************************---BuildEquivalenceClass---**************************************/
void PushNeighbour(struct Node *m, struct Node *c) {
    PushToListHT(m->neighbour, c);
    PushToListHT(c->neighbour, m);
}

void Propagate(struct Node *m, struct Node *c) {//propagazione nodi....
    switch (m->label) {
        case FVar:
            if (c->label == FVar) {
                //tali if innestati servono per non avere un NullPointErexception e controllare che i canonici dei binder siano uguali
                if (m->content.fvar.var != NULL) {
                    if (c->content.fvar.var == NULL ||
                        m->content.fvar.var->canonic != c->content.fvar.var->canonic)
                        PrintExit(3);
                } else {
                    if (c->content.fvar.var != NULL)
                        PrintExit(3);
                }
            } else
                PrintExit(3);
            break;
        case BVar:
            if (c->label == BVar) {
                //tali if innestati servono per non avere un NullPointErexception e controllare che i canonici dei binder siano uguali
                if (m->content.bvar.binder != NULL) {
                    if (c->content.bvar.binder == NULL ||
                        m->content.bvar.binder->canonic != c->content.bvar.binder->canonic)
                        PrintExit(3);
                } else {
                    if (c->content.bvar.binder != NULL)
                        PrintExit(3);
                }
            } else
                PrintExit(3);
            break;
        case Shared:
            break; //todo break or fail???
        case App:
            if (c->label == App) {
                PushNeighbour(m->content.app.left, c->content.app.left);
                PushNeighbour(m->content.app.right, c->content.app.right);
            } else
                PrintExit(3);
            break;
        case Lam:
            if (c->label == Lam)
                PushNeighbour(m->content.lam.body, c->content.lam.body);
            else
                PrintExit(3);
            break;
        case Match:
            if (c->label == Match)
                PushNeighbour(m->content.match.body, c->content.match.body);
            else
                PrintExit(3);
            break;
        case Let://non dovrebbe propagarsi
            break; //todo break or fail???
        case FRic://todo PushNeighbour anche arg???
            if (c->label == FRic)
                PushNeighbour(m->content.fRic.t, c->content.fRic.t);
            else
                PrintExit(3);
            break;
        case GCoRic://non dovrebbe propagarsi
            break; //todo break or fail???
        case Constructor://todo necessario??? -> Push Neighbour tutti arg dei 2 nodi Constructor
            if (c->label == Constructor && m->content.jCostr.j==c->content.jCostr.j){
                struct ListElement *iter = m->content.jCostr.arg->head;
                struct ListElement *iter2 = c->content.jCostr.arg->head;
                for (int i = 0; i < m->content.jCostr.arg->count; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            }else
                PrintExit(3);
            break;
        case Constant://non dovrebbe propagarsi
            break; //todo break or fail???
    }
}

void Enqueue(struct Node *n, struct Node *c) {
    if (n->canonic == NULL) {
        PushToListHT(c->queue, n);
        n->canonic = c;
    } else if (n->canonic != c)
        PrintExit(2);
}

void BuildClass(struct Node *c) {
    printf("BuildClass Node, Label= %d\n", c->label);
    c->building = True;
    c->queue = InitListHT();
    struct ListElement *iterQueue = c->queue->head;
    Enqueue(c, c);
    int count = 0;
    while (c->queue->count > count) {
        count++;
        struct Node *n = iterQueue->node;
        if (n->parentNodes != NULL) {// visit parents
            struct ListElement *iterParents = n->parentNodes->head;
            for (int i = 0; i < n->parentNodes->count; ++i) {
                if (iterParents->node->canonic == NULL)
                    BuildClass(iterParents->node);
                else if (iterParents->node->canonic->building == True)
                    PrintExit(2);
                iterParents = iterParents->next;
            }
        }
        if (n->neighbour == NULL || n->neighbour->count == 0) {
            assert(n == c);
            assert(c->queue->count <= count);
            c->building = False;
            return;
        }
        if (n->neighbour != NULL) {// visit neighbours
            struct ListElement *iterNeighbour = n->neighbour->head;
            for (int i = 0; i < n->neighbour->count; ++i) {
                Enqueue(iterNeighbour->node, c);
                iterNeighbour = iterNeighbour->next;
            }
        }
        if (n->label == Lam) {
            n->content.lam.body = WeakCbVEval(n->content.lam.body);
        }
        Propagate(n, c);//Propagate
        iterQueue = iterQueue->next;
    }
    c->building = False;
}

/*****************************************--- Main-Equivalence&Eval---**************************************/
void DAGCheckAndEval(struct ListHT *nodesHT) {
    printf("START DAGCheckAndEval\n");
    struct ListElement *nodes = nodesHT->head;
    for (int i = 0; i < nodesHT->count; ++i) {
        if (nodes->node->canonic == NULL)
            BuildClass(nodes->node);
        nodes = nodes->next;
    }
    printf("END DAGCheckAndEval\n");
}