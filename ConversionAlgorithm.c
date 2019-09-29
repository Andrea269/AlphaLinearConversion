//
// Created by andrea on 17/07/19.
//


//todo piai propagazione uguale a lamdba... riduzione nulla vado si figli

// todo spostare valu

//todo count

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/********---DATA STRUCTURES---*************/
struct ListHT;
struct ListHT *nodesHT;//Node List

enum TypeNode {
    FVar, BVar, Shared, App, Lam, Match, Let, FRic, GCoRic, Constructor, Constant
};
enum Bool {
    True, False
};

struct NodeFVar {
    int var;//dummy
};
struct NodeBVar {
    struct Node *binder;//NodeLam or NodeLet or NodeFRic or NodeGCoRic
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
    struct Node *var;//NodeBVar
    struct Node *t;
    int n;
    struct ListHT *arg;
};
struct NodeMatch {
//    struct NodeType *i; //tipo su cui faccio match
//    struct NodeType *t0;// il tipo dei rami
    struct Node *body;
    struct ListHT *branches; //lista dei rami la cui lunghezza dipende dal tipo
};
struct NodeLet {
    struct Node *var;//NodeBVar
//    struct NodeType *t1;
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
    enum Bool root;
    enum Bool reachable;
    enum Bool visited;
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

/******************************---INIT-NODE---*************************************/
struct Node *InitFVar() {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = FVar;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(nodesHT, node);
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
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(nodesHT, node);
    return node;
}

struct Node *InitShared(struct Node *body) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Shared;
    node->content.shared.body = body;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(nodesHT, node);
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
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(left->parentNodes, node);
    PushToListHT(right->parentNodes, node);
    PushToListHT(nodesHT, node);
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
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    PushToListHT(nodesHT, node);
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
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    PushToListHT(nodesHT, node);
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
    node->content.fRic.t = body;
    node->content.fRic.n = n;
    node->content.fRic.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
    PushParentListHT(node, arg);
    PushToListHT(nodesHT, node);
    return node;
}

struct Node *InitGCoRic(struct Node *var, struct Node *body, int n, struct ListHT *arg) {
    struct Node *node = malloc(sizeof(struct Node));
    node->label = GCoRic;
    node->content.gCoRic.var = var;
    node->content.gCoRic.t = body;
    node->content.gCoRic.n = n;
    node->content.gCoRic.arg = arg;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, arg);
    PushToListHT(nodesHT, node);
    return node;
}

struct Node *InitMatch(struct Node *body, struct ListHT *branches) {
    if (!(body->label == Constructor || body->label == GCoRic)) {
        printf("Exit InitMatchConstructor-> error in un nodo Match il nodo body deve essere un Constructor o un GCoRic \n");
        PrintExit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Match;
//    node->content.match.i = i;
//    node->content.match.t0 = t0;
    node->content.match.body = body;
    node->content.match.branches = branches;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushParentListHT(node, branches);
    PushToListHT(body->parentNodes, node);
    PushToListHT(nodesHT, node);
    return node;
}

struct Node *InitLet(struct Node *var, struct Node *t2, struct Node *t3) {
    if (var->label != BVar) {
        printf("Exit InitLet-> error in un nodo Let il nodo var deve essere un Var\n");
        PrintExit(1);
    }
    struct Node *node = malloc(sizeof(struct Node));
    node->label = Let;
    node->content.let.var = var;
//    node->content.let.t1 = t1;
    node->content.let.t2 = t2;
    node->content.let.t3 = t3;
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(t2->parentNodes, node);
    PushToListHT(t3->parentNodes, node);
    PushToListHT(nodesHT, node);
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
            n1 = InitLam(InitBVar(NULL), n->content.lam.body);
            n->copy = n1;
            n1->content.lam.body = Inst(n->content.lam.body, l, sub);
            n->copy = NULL;
            return n1;
        case Match:
            return n;
        case Let:
            n1 = InitLet(InitBVar(NULL), n->content.let.t2, n->content.let.t3);
            n->copy = n1;
            n1->content.let.t2 = Inst(n->content.let.t2, l, sub);
            n1->content.let.t3 = Inst(n->content.let.t3, l, sub);
            n->copy = NULL;
            return n1;
        case FRic:
            n1 = InitFRic(InitBVar(NULL), n->content.fRic.t, n->content.fRic.n, n->content.fRic.arg);
            n->copy = n1;
            n1->content.fRic.t = Inst(n->content.fRic.t, l, sub);
            struct ListElement *iterArgf = n->content.fRic.arg->head;
            for (int i = 0; i < n->content.fRic.arg->count; ++i) {
                iterArgf->node = Inst(iterArgf->node, l, sub);
                iterArgf = iterArgf->next;
            }
            n->copy = NULL;
            return n1;
        case GCoRic:
            n1 = InitGCoRic(InitBVar(NULL), n->content.gCoRic.t, n->content.gCoRic.n, n->content.gCoRic.arg);
            n->copy = n1;
            n1->content.gCoRic.t = Inst(n->content.gCoRic.t, l, sub);
            struct ListElement *iterArgG = n->content.gCoRic.arg->head;
            for (int i = 0; i < n->content.gCoRic.arg->count; ++i) {
                iterArgG->node = Inst(iterArgG->node, l, sub);
                iterArgG = iterArgG->next;
            }
            n->copy = NULL;
            return n1;
        case Constructor:
            n1 = InitConstructor(n->content.jCostr.j, n->content.jCostr.arg);
            struct ListElement *iterArgC = n1->content.jCostr.arg->head;
            for (int i = 0; i < n1->content.jCostr.arg->count; ++i) {
                iterArgC->node = Inst(iterArgC->node, l, sub);
                iterArgC = iterArgC->next;
            }
            return n1;
        case Constant:
            return n;
    }
}

struct Node *AppReplacement(struct Node *tj, struct ListHT *args) {
    struct Node *result = tj;
    struct ListElement *arg = args->head;
    for (int i = 0; i < args->count; ++i) {
        result = InitApp(result, arg->node);
        arg = arg->next;
    }
    return result;
}

void UpdateSon(struct Node *oldSon, struct Node *newSon, struct Node *parent) {
    int find = 0;
    switch (parent->label) {
        case FVar:
            assert(0);
        case BVar:
            if (parent->content.bvar.binder == oldSon)
                parent->content.bvar.binder = newSon;
            else
                assert(0);
            break;
        case Shared:
            if (parent->content.shared.body == oldSon)
                parent->content.shared.body = newSon;
            else
                assert(0);
            break;
        case App:
            if (parent->content.app.left == oldSon)
                parent->content.app.left = newSon;
            else if (parent->content.app.right == oldSon)
                parent->content.app.right = newSon;
            else
                assert(0);
            break;
        case Lam:
            if (parent->content.lam.body == oldSon)
                parent->content.lam.body = newSon;
            else if (parent->content.lam.var == oldSon)
                parent->content.lam.var = newSon;
            else
                assert(0);
            break;
        case Match:
            if (parent->content.match.body == oldSon)
                parent->content.match.body = newSon;
            else {
                if (parent->content.match.branches != NULL) {
                    struct ListElement *branches = parent->content.match.branches->head;
                    int i = 0;
                    while (find == 0 && i < parent->content.match.branches->count) {
                        if (branches->node == oldSon) {
                            branches->node = newSon;
                            find = 1;
                        }
                        ++i;
                        branches = branches->next;
                    }
                }
                if (find == 0)
                    assert(0);
            }
            break;
        case Let:
            if (parent->content.let.t3 == oldSon)
                parent->content.let.t3 = newSon;
            else if (parent->content.let.t2 == oldSon)
                parent->content.let.t2 = newSon;
            else if (parent->content.let.var == oldSon)
                parent->content.let.var = newSon;
            else
                assert(0);
            break;
        case FRic:
            if (parent->content.fRic.t == oldSon)
                parent->content.fRic.t = newSon;
            else if (parent->content.fRic.var == oldSon)
                parent->content.fRic.var = newSon;
            else {
                if (parent->content.fRic.arg != NULL) {
                    struct ListElement *arg = parent->content.fRic.arg->head;
                    int i = 0;
                    while (find == 0 && i < parent->content.fRic.arg->count) {
                        if (arg->node == oldSon) {
                            arg->node = newSon;
                            find = 1;
                        }
                        ++i;
                        arg = arg->next;
                    }
                }
                if (find == 0)
                    assert(0);
            }
            break;
        case GCoRic:
            if (parent->content.gCoRic.t == oldSon)
                parent->content.gCoRic.t = newSon;
            else if (parent->content.gCoRic.var == oldSon)
                parent->content.gCoRic.var = newSon;
            else {
                if (parent->content.gCoRic.arg != NULL) {
                    struct ListElement *arg = parent->content.gCoRic.arg->head;
                    int i = 0;
                    while (find == 0 && i < parent->content.gCoRic.arg->count) {
                        if (arg->node == oldSon) {
                            arg->node = newSon;
                            find = 1;
                        }
                        ++i;
                        arg = arg->next;
                    }
                }
                if (find == 0)
                    assert(0);
            }
            break;
        case Constructor:
            if (parent->content.jCostr.arg != NULL) {
                struct ListElement *arg = parent->content.jCostr.arg->head;
                int i = 0;
                while (find == 0 && i < parent->content.jCostr.arg->count) {
                    if (arg->node == oldSon) {
                        arg->node = newSon;
                        find = 1;
                    }
                    ++i;
                    arg = arg->next;
                }
            }
            if (find == 0)
                assert(0);
            break;
        case Constant:
            if (parent->content.constant.var == oldSon)
                parent->content.constant.var = newSon;
            else
                assert(0);
            break;
        default:
            assert(0);

    }
}

void RefactoringArcs(struct Node *oldSon, struct Node *newSon) {
    struct ListElement *parent = oldSon->parentNodes->head;
    for (int i = 0; i < oldSon->parentNodes->count; ++i) {
        //aggiorno il nuovo figlio nel genitore
        UpdateSon(oldSon, newSon, parent->node);
        //aggiorno i parent del nuovo nodo preservando i vecchi parent
        PushToListHT(newSon->parentNodes, parent->node);
        parent = parent->next;
    }
}

struct Node *WeakCbVEval(struct Node *n) {//riduzione e aggiornamento archi Padre-Figlio
    printf("Eval Node, Label= %d\n", n->label);
    struct ListElement *listElement;
    struct Node *n1;
    struct Node *n2;
    struct Node *nodeReturn;
    switch (n->label) {
        case FVar:
            return n;
        case BVar:
            return n;
        case Shared:
            nodeReturn = n->content.shared.body;

            RefactoringArcs(n, nodeReturn);
            return nodeReturn;
        case App:
            n2 = WeakCbVEval(n->content.app.right);
            n1 = WeakCbVEval(n->content.app.left);
            if (n1->label == Lam)
                nodeReturn = WeakCbVEval(Inst(n1->content.lam.body, n1, InitShared(n2)));
            else
                nodeReturn = InitApp(n1, n2);

            RefactoringArcs(n, nodeReturn);
            return nodeReturn;
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
                    n1 = WeakCbVEval(listElement->node);
                    n2 = AppReplacement(n1, n->content.match.body->content.jCostr.arg);
                    nodeReturn = WeakCbVEval(n2);
                    break;
                case GCoRic://jDelta-c
                    n1 = Inst(n->content.match.body->content.gCoRic.t, n->content.match.body->content.gCoRic.var,
                              n->content.match.body);
                    n->content.match.body = AppReplacement(n1, n->content.match.body->content.gCoRic.arg);
                    nodeReturn = WeakCbVEval(n);
                    break;
                default:
                    return n;
            }

            RefactoringArcs(n, nodeReturn);
            return nodeReturn;
        case Let:
            n->content.let.var->content.bvar.binder = WeakCbVEval(n->content.let.t2);
            n1 = n->content.let.t3;
            nodeReturn = WeakCbVEval(n1);

            RefactoringArcs(n, nodeReturn);
            return nodeReturn;
        case FRic:
            listElement = n->content.fRic.arg->head;
            for (int i = 0; i < n->content.fRic.arg->count - 1; ++i) {
                listElement = listElement->next;
            }
            //valuto l'arg j-esimo
            listElement->node = WeakCbVEval(listElement->node);
            if (listElement->node->label == Constructor) {
                n1 = Inst(n->content.fRic.t, n->content.fRic.var, n);//b
                nodeReturn = WeakCbVEval(AppReplacement(n1, n->content.fRic.arg));

                RefactoringArcs(n, nodeReturn);
                return nodeReturn;
            } else {
                n->content.fRic.t = WeakCbVEval(n->content.fRic.t);
                return n;
            }
        case GCoRic:
            return n;
        case Constructor:
            return n;
        case Constant:
            nodeReturn = n->content.constant.var;

            RefactoringArcs(n, nodeReturn);
            return nodeReturn;
    }
}

/***************************************---BuildEquivalenceClass---**************************************/
void PushNeighbour(struct Node *m, struct Node *c) {


    struct Node *m1 = WeakCbVEval(m);
    struct Node *c1 = WeakCbVEval(c);

    if(m->root==True)
        m1->root=True;
    if(c->root==True)
        c1->root=True;

    PushToListHT(m1->neighbour, c1);
    PushToListHT(c1->neighbour, m1);
}


void RecWCEval(struct Node *n) {
    n->reachable = n->root;

    if (n->parentNodes != NULL) {// visit parents
        struct ListElement *m = n->parentNodes->head;
        for (int i = 0; i < n->parentNodes->count; ++i) {
            if (m->node->visited == False)
                RecWCEval(m->node);
            //or logico, False cases the value of n->reachable does't change -- cases True is equivalent to assignment
            if (m->node->reachable == True)
                n->reachable = True;
            m = m->next;
        }
    }

    n->visited = True;
}

/**
 * E’ giusto che il Let non si propaghi
 * (gli invarianti dicono che non dovresti mai incontrarlo a quel punto del codice).
 * Invece FRic, GCoRic si propagano e
 * vanno aggiunti archi ~ tra tutti i sotto-nodi corrispondenti, come nel caso del Constructor. Si’ anche i rami.
 * */
void Propagate(struct Node *m, struct Node *c) {//propagazione nodi....
    switch (m->label) {
        case FVar:
            if (m != c)
                PrintExit(3);
            break;
        case BVar:
            if (c->label == BVar) {
                assert(m->content.bvar.binder != NULL);
                assert(c->content.bvar.binder != NULL);
                if (m->content.bvar.binder->canonic != c->content.bvar.binder->canonic)
                    PrintExit(3);
            } else
                PrintExit(3);
            break;
        case Shared://un nodo Shared non dovrebbe mai incorrere in questo punto del codice
            assert(0);
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
            if (c->label == Match && m->content.match.branches->count == c->content.match.branches->count) {
                PushNeighbour(m->content.match.body, c->content.match.body);
                struct ListElement *iter = m->content.match.branches->head;
                struct ListElement *iter2 = c->content.match.branches->head;
                for (int i = 0; i < m->content.match.branches->count; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case Let://un nodo LET non dovrebbe mai incorrere in questo punto del codice
            assert(0);
        case FRic:
            if (c->label == FRic && m->content.fRic.arg->count == c->content.fRic.arg->count) {
                PushNeighbour(m->content.fRic.t, c->content.fRic.t);
                struct ListElement *iter = m->content.fRic.arg->head;
                struct ListElement *iter2 = c->content.fRic.arg->head;
                for (int i = 0; i < m->content.fRic.arg->count; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case GCoRic:
            if (c->label == GCoRic && m->content.gCoRic.arg->count == c->content.gCoRic.arg->count) {
                PushNeighbour(m->content.gCoRic.t, c->content.gCoRic.t);
                struct ListElement *iter = m->content.gCoRic.arg->head;
                struct ListElement *iter2 = c->content.gCoRic.arg->head;
                for (int i = 0; i < m->content.gCoRic.arg->count; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case Constructor://j indica quale costruttore è
            if (c->label == Constructor && m->content.jCostr.j == c->content.jCostr.j &&
                m->content.jCostr.arg->count == c->content.jCostr.arg->count) {
                struct ListElement *iter = m->content.jCostr.arg->head;
                struct ListElement *iter2 = c->content.jCostr.arg->head;
                for (int i = 0; i < m->content.jCostr.arg->count; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case Constant://un nodo Constant non dovrebbe mai incorrere in questo punto del codice
            assert(0);
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
        RecWCEval(n);
        if (n->reachable == False) {
//todo????            assert(n->neighbour == NULL || n->neighbour->count <= count);
            c->building = False;
            return;
        }
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
        if (n->neighbour != NULL) {// visit neighbours
            struct ListElement *iterNeighbour = n->neighbour->head;
            for (int i = 0; i < n->neighbour->count; ++i) {
                Enqueue(iterNeighbour->node, c);
                iterNeighbour = iterNeighbour->next;
            }
        }
        if (n != c)
            Propagate(n, c);
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