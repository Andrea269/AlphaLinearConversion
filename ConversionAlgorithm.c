//
// Created by andrea on 17/07/19.
//
// graphviz come pacchetto
// gcc -o main main.c && ./main ; dot -Tpdf graph.dot > graph.pdf
// && evince graph.pdf
//
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/********---DATA STRUCTURES---*************/

typedef struct Node Node;
typedef struct List List;
typedef struct ListItem ListItem;
typedef enum Bool Bool;

List *nodesHT;//Node List

enum TypeNode {
    FVar, BVar, Piai, Shared, App, Lam, Match, Let, FRic, GCoRic, Constructor, Constant
};
enum Bool {
    True, False
};
struct NodeFVar {
    int var;//dummy
};
struct NodeBVar {
    Node *binder;//NodeLam or NodeLet or NodeFRic or NodeGCoRic
};
struct NodePiai {
    Node *var;
    Node *body;
};
struct NodeShared {
    Node *body;
};
struct NodeApp {
    Node *left;
    Node *right;
};
struct NodeLam {
    Node *var;//NodeBVar
    Node *body;
};
struct NodeJthConstr {
    int j;
    List *arg;
    int n;
};
struct NodeFRic {
    Node *var;//NodeBVar
    Node *t;
    int n;
    List *arg;
};
struct NodeGCoRic {
    Node *var;//NodeBVar
    Node *t;
    int n;
    List *arg;
};
struct NodeMatch {
    Node *body;
    List *branches; //lista dei rami la cui lunghezza dipende dal tipo
    int n;
};
struct NodeLet {
    Node *var;//NodeBVar
    Node *t2;
    Node *t3;
};
struct NodeConst {
    Node *var;
};
struct Node {
    enum TypeNode label;
    union {
        struct NodeFVar fvar;
        struct NodeBVar bvar;
        struct NodePiai piai;
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
    Node *canonic;
    Node *copy;
    Bool building;
    List *parentNodes;
    List *neighbour;
    List *queue;
    Bool root;
    Bool reachable;
    Bool visited;
    int rc;
};
struct ListItem {
    Node *node;
    ListItem *next;
};
struct List {
    ListItem *head;
    ListItem *tail;
};

void FreeRC(Node *node);

/*****************************---PRINTING ---************************************/
const char *print_label(enum TypeNode l) {
    switch (l) {
        case FVar:
            return "v";
        case BVar:
            return "b";
        case Piai:
            return "pi";
        case Shared:
            return "S";
        case App:
            return "@";
        case Lam:
            return "\\\\";
        case Match:
            return "match";
        case Let:
            return "let";
        case FRic:
            return "fix";
        case GCoRic:
            return "cofix";
        case Constructor:
            return "k";
        case Constant:
            return "c";
    }
}

void print_node(FILE *f, Node *n) {
    fprintf(f, "%d [label=\"%s\" ordering=\"out\"];\n", (long) n, print_label(n->label));
    ListItem *listElement = n->parentNodes->head;
    while (listElement->node != NULL) {
        fprintf(f, "%d -> %d;\n", (long) listElement->node, (long) n);
        listElement = listElement->next;
    }
    listElement = n->neighbour->head;
    while (listElement->node != NULL) {
        fprintf(f, "%d -> %d [style=\"dotted\" constraint=\"false\"];\n", (long) listElement->node, (long) n);
        listElement = listElement->next;
    }
    if (n->label == BVar && n->content.bvar.binder != NULL)
        fprintf(f, "%d -> %d;\n", (long) n, (long) n->content.bvar.binder);

    if (n->root == True) fprintf(f, "%d [shape=\"square\"];\n", (long) n);
}

void print_graph(List *l, Node *focus) {
    fprintf(stderr, "----- new graph -----\n");
    FILE *f = fopen("graph.dot", "w");
    fprintf(f, "digraph G {\n");
    ListItem *listElement = l->head;
    while (listElement->node != NULL) {
        if (listElement->node->rc > 0 || listElement->node->root == True)
            print_node(f, listElement->node);
        listElement = listElement->next;
    }
    if (focus != NULL) fprintf(f, "%d [color=\"red\"];\n", (long) focus);
    fprintf(f, "}\n");
    fclose(f);
    system("dot -Tpdf graph.dot > graph.pdf");
    getchar();
}

/*****************************---USEFUL FUNCTIONS---************************************/
void PrintExit(int error) {
    printf("\nFail: Process finished with exit code %d\n", error);
    exit(error);
}

List *InitListHT() {
    List *list = malloc(sizeof(List));
    ListItem *elementList = malloc(sizeof(ListItem));
    elementList->node = NULL;
    elementList->next = NULL;
    list->head = elementList;
    list->tail = elementList;
    return list;
};

void PushToListHT(List *list, Node *prtNode) {
    ListItem *elementList = malloc(sizeof(ListItem));
    elementList->node = NULL;
    elementList->next = NULL;
    list->tail->node = prtNode;
    list->tail->next = elementList;
    list->tail = list->tail->next;// setto la nuova coda
};

void RemoveHT(List *list, Node *prtNode) {
    ListItem *listElement = list->head;
    ListItem *prev = NULL;
    while (listElement->node != NULL) {
        if (listElement->node == prtNode) {
            if (prev != NULL) {
                prev->next = listElement->next;
            } else {
                list->head = listElement->next;
            }
            break;
        } else {
            prev = listElement;
            listElement = listElement->next;
        }
    }
}

void PushParentListHT(Node *node, List *list) {
    ListItem *listElement = list->head;
    while (listElement->node != NULL) {
        PushToListHT(listElement->node->parentNodes, node);
        listElement->node->rc++;
        listElement = listElement->next;
    }
}

/******************************---INIT-NODE---*************************************/
void InitBase(Node *node) {
    node->canonic = NULL;
    node->copy = NULL;
    node->building = False;
    node->root = False;
    node->visited = False;
    node->rc = 0;
    node->parentNodes = InitListHT();
    node->neighbour = InitListHT();
    PushToListHT(nodesHT, node);
}

Node *InitFVarId(int i) {
    Node *node = malloc(sizeof(Node));
    node->label = FVar;
    node->content.fvar.var = i;
    InitBase(node);
    return node;
}

Node *InitBVar(Node *binder) {
    Node *node = malloc(sizeof(Node));
    node->label = BVar;
    node->content.bvar.binder = binder;//no RC
    InitBase(node);
    return node;
}

Node *InitPiai(Node *var, Node *body) {
    Node *node = malloc(sizeof(Node));
    node->label = Piai;
    node->content.piai.var = var;
    var->rc++;
    node->content.piai.body = body;
    body->rc++;
    InitBase(node);
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    return node;
}

Node *InitShared(Node *body) {
    Node *node = malloc(sizeof(Node));
    node->label = Shared;
    node->content.shared.body = body;
    body->rc++;
    PushToListHT(body->parentNodes, node);
    InitBase(node);
    return node;
}

Node *InitApp(Node *left, Node *right) {
    Node *node;
    switch (left->label) {
        case FRic:
            PushToListHT(left->content.fRic.arg, right);
            right->rc++;
            break;
        case GCoRic:
            PushToListHT(left->content.gCoRic.arg, right);
            right->rc++;
            break;
        default:
            node = malloc(sizeof(Node));
            node->label = App;
            node->content.app.left = left;
            left->rc++;
            node->content.app.right = right;
            right->rc++;
            InitBase(node);
            PushToListHT(left->parentNodes, node);
            PushToListHT(right->parentNodes, node);
            return node;
    }
}

Node *InitLam(Node *var, Node *body) {
    if (var->label != BVar) {
        printf("Exit InitLam-> error in un nodo Lam il nodo sinistro deve essere un BVar\n");
        PrintExit(1);
    }
    Node *node = malloc(sizeof(Node));
    node->label = Lam;
    node->content.lam.var = var;
    var->rc++;
    node->content.lam.body = body;
    body->rc++;
    InitBase(node);
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    return node;
}

Node *InitConstructor(int j, List *arg, int n) {
    Node *node = malloc(sizeof(Node));
    node->label = Constructor;
    node->content.jCostr.j = j;
    node->content.jCostr.arg = arg;
    node->content.jCostr.n = n;
    InitBase(node);
    PushParentListHT(node, arg);
    return node;
}

Node *InitFRic(Node *var, Node *body, int n, List *arg) {
    if (var->label != BVar) {
        printf("Exit InitLam-> error in un nodo FRic il nodo var deve essere un BVar\n");
        PrintExit(1);
    }
    Node *node = malloc(sizeof(Node));
    node->label = FRic;
    node->content.fRic.var = var;
    var->rc++;
    node->content.fRic.t = body;
    body->rc++;
    node->content.fRic.n = n;
    node->content.fRic.arg = arg;
    InitBase(node);
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    PushParentListHT(node, arg);
    return node;
}

Node *InitGCoRic(Node *var, Node *body, int n, List *arg) {
    Node *node = malloc(sizeof(Node));
    node->label = GCoRic;
    node->content.gCoRic.var = var;
    var->rc++;
    node->content.gCoRic.t = body;
    body->rc++;
    node->content.gCoRic.n = n;
    node->content.gCoRic.arg = arg;
    InitBase(node);
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(body->parentNodes, node);
    PushParentListHT(node, arg);
    return node;
}

Node *InitMatch(Node *body, List *branches, int n) {
    if (!(body->label == Constructor || body->label == GCoRic)) {
        printf("Exit InitMatchConstructor-> error in un nodo Match il nodo body deve essere un Constructor o un GCoRic \n");
        PrintExit(1);
    }
    Node *node = malloc(sizeof(Node));
    node->label = Match;
    node->content.match.body = body;
    body->rc++;
    node->content.match.branches = branches;
    node->content.match.n = n;
    InitBase(node);
    PushToListHT(body->parentNodes, node);
    PushParentListHT(node, branches);
    return node;
}

Node *InitLet(Node *var, Node *t2, Node *t3) {
    if (var->label != BVar) {
        printf("Exit InitLet-> error in un nodo Let il nodo var deve essere un Var\n");
        PrintExit(1);
    }
    Node *node = malloc(sizeof(Node));
    node->label = Let;
    node->content.let.var = var;
    var->rc++;
    node->content.let.t2 = t2;
    t2->rc++;
    node->content.let.t3 = t3;
    t3->rc++;
    InitBase(node);
    var->content.bvar.binder = node;
    PushToListHT(var->parentNodes, node);
    PushToListHT(t2->parentNodes, node);
    PushToListHT(t3->parentNodes, node);
    return node;
}


/*************************************---EVALUATE---*****************************************/
void RefactoringNode(Node *oldNode, Node *newNode);

Node *Inst(Node *n, Node *l, Node *sub) {
    Node *n1;
    Node *temp;
    ListItem *iterList;
    struct List *arg = InitListHT();
    switch (n->label) {
        case FVar:
            return n;
        case BVar:
            if (n->content.bvar.binder == l)
                return InitShared(sub);
            else {
                if (n->content.bvar.binder->copy == NULL)
                    return n;
                else
                    return InitBVar(n->content.bvar.binder->copy);
            }
        case Piai:
            n1 = InitPiai(InitBVar(NULL), n->content.piai.body);
            n->copy = n1;
            temp = n1->content.piai.body;
            n1->content.piai.body = Inst(temp, l, sub);
            if (temp != n1->content.piai.body) {
                PushToListHT(n1->content.piai.body->parentNodes, n1);
                n1->content.piai.body->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            n->copy = NULL;
            return n1;
        case Shared:
            return n;
        case App:
            return InitApp(Inst(n->content.app.left, l, sub), Inst(n->content.app.right, l, sub));
        case Lam:
            n1 = InitLam(InitBVar(NULL), n->content.lam.body);
            n->copy = n1;

            temp = n1->content.lam.body;
            n1->content.lam.body = Inst(temp, l, sub);
            if (temp != n1->content.lam.body) {
                PushToListHT(n1->content.lam.body->parentNodes, n1);
                n1->content.lam.body->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            n->copy = NULL;
            return n1;
        case Match:
            return n;
        case Let:
            n1 = InitLet(InitBVar(NULL), n->content.let.t2, n->content.let.t3);
            n->copy = n1;
            temp = n1->content.let.t2;
            n1->content.let.t2 = Inst(temp, l, sub);
            if (temp != n1->content.let.t2) {
                PushToListHT(n1->content.let.t2->parentNodes, n1);
                n1->content.let.t2->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            temp = n1->content.let.t3;
            n1->content.let.t3 = Inst(temp, l, sub);
            if (temp != n1->content.let.t3) {
                PushToListHT(n1->content.let.t3->parentNodes, n1);
                n1->content.let.t3->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            n->copy = NULL;
            return n1;
        case FRic:
            iterList = n->content.fRic.arg->head;
            while (iterList->node != NULL) {
                PushToListHT(arg, iterList->node);
                iterList = iterList->next;
            }

            n1 = InitFRic(InitBVar(NULL), n->content.fRic.t, n->content.fRic.n, arg);
            n->copy = n1;
            temp = n1->content.fRic.t;
            n1->content.fRic.t = Inst(temp, l, sub);
            if (temp != n1->content.fRic.t) {
                PushToListHT(n1->content.fRic.t->parentNodes, n1);
                n1->content.fRic.t->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            iterList = n1->content.fRic.arg->head;
            while (iterList->node != NULL) {
                temp = iterList->node;
                iterList->node = Inst(temp, l, sub);
                if (temp != iterList->node) {
                    PushToListHT(iterList->node->parentNodes, n1);
                    iterList->node->rc++;
                    RemoveHT(temp->parentNodes, n1);
                    temp->rc--;
                }
                iterList = iterList->next;
            }
            n->copy = NULL;
            return n1;
        case GCoRic:
            iterList = n->content.gCoRic.arg->head;
            while (iterList->node != NULL) {
                PushToListHT(arg, iterList->node);
                iterList = iterList->next;
            }
            n1 = InitGCoRic(InitBVar(NULL), n->content.gCoRic.t, n->content.gCoRic.n, arg);
            n->copy = n1;
            temp = n1->content.gCoRic.t;
            n1->content.gCoRic.t = Inst(temp, l, sub);
            if (temp != n1->content.gCoRic.t) {
                PushToListHT(n1->content.gCoRic.t->parentNodes, n1);
                n1->content.gCoRic.t->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            iterList = n1->content.gCoRic.arg->head;
            while (iterList->node != NULL) {
                temp = iterList->node;
                iterList->node = Inst(temp, l, sub);
                if (temp != iterList->node) {
                    PushToListHT(iterList->node->parentNodes, n1);
                    iterList->node->rc++;
                    RemoveHT(temp->parentNodes, n1);
                    temp->rc--;
                }
                iterList = iterList->next;
            }
            n->copy = NULL;
            return n1;
        case Constructor:
            iterList = n->content.jCostr.arg->head;
            while (iterList->node != NULL) {
                PushToListHT(arg, iterList->node);
                iterList = iterList->next;
            }
            n1 = InitConstructor(n->content.jCostr.j, arg, n->content.jCostr.n);
            iterList = n1->content.jCostr.arg->head;
            while (iterList->node != NULL) {
                temp = iterList->node;
                iterList->node = Inst(temp, l, sub);
                if (temp != iterList->node) {
                    PushToListHT(iterList->node->parentNodes, n1);
                    iterList->node->rc++;
                    RemoveHT(temp->parentNodes, n1);
                    temp->rc--;
                }
                iterList = iterList->next;
            }
            return n1;
        case Constant:
            return n;
    }
}

Node *AppReplacement(Node *tj, List *args) {
    Node *result = tj;
    ListItem *arg = args->head;
    while (arg->node != NULL) {
        result = InitApp(result, arg->node);
        arg = arg->next;
    }
    return result;
}

void UpdateSon(Node *oldSon, Node *newSon, Node *parent) {
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
        case Piai:
            if (parent->content.piai.body == oldSon)
                parent->content.piai.body = newSon;
            else if (parent->content.piai.var == oldSon)
                parent->content.piai.var = newSon;
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
                assert(parent->content.match.branches != NULL);
                ListItem *branches = parent->content.match.branches->head;
                while (find == 0 && branches->node != NULL) {
                    if (branches->node == oldSon) {
                        branches->node = newSon;
                        find = 1;
                    }
                    branches = branches->next;
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
                assert(parent->content.fRic.arg != NULL);
                ListItem *arg = parent->content.fRic.arg->head;
                int i = 0;
                while (find == 0 && arg->node != NULL) {
                    if (arg->node == oldSon) {
                        arg->node = newSon;
                        find = 1;
                    }
                    ++i;
                    arg = arg->next;
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
                assert(parent->content.gCoRic.arg != NULL);
                ListItem *arg = parent->content.gCoRic.arg->head;
                while (arg->node != NULL) {
                    if (arg->node == oldSon) {
                        arg->node = newSon;
                        find = 1;
                    }
                    arg = arg->next;
                }
                if (find == 0)
                    assert(0);
            }
            break;
        case Constructor:
            assert(parent->content.jCostr.arg != NULL);
            ListItem *arg = parent->content.jCostr.arg->head;
            while (find == 0 && arg->node != NULL) {
                if (arg->node == oldSon) {
                    arg->node = newSon;
                    find = 1;
                }
                arg = arg->next;
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

/*Aggiorna il RC del nodo passato in input, rimuove il parent dalla sua lista di genitori
 * Se il nodo non ha più alcun riferimento effettua una chiamata mutuamente ricorsiva alla funzione FreeRC*/
void RefactoringSon(Node *node, Node *parent) {
    node->rc--;
    RemoveHT(node->parentNodes, parent);
    if (node->root == False && node->rc < 1)
        FreeRC(node);
}

/*Rimuove il nodo passato in input dalla lista globale e libera la memoria occupata da tale nodo
 * Prima di liberare la memoria invoca la funzione RefactoringSon su tutti i figli del nodo*/
void FreeRC(Node *node) {
    RemoveHT(nodesHT, node);
    ListItem *listElement;
    switch (node->label) {
        case FVar:
            break;
        case BVar:
            break;
        case Piai:
            RefactoringSon(node->content.piai.var, node);
            RefactoringSon(node->content.piai.body, node);
            break;
        case Shared:
            RefactoringSon(node->content.shared.body, node);
            break;
        case App:
            RefactoringSon(node->content.app.left, node);
            RefactoringSon(node->content.app.right, node);
            break;
        case Lam:
            RefactoringSon(node->content.lam.body, node);
            RefactoringSon(node->content.lam.var, node);
            break;
        case Match:
            RefactoringSon(node->content.match.body, node);
            listElement = node->content.match.branches->head;
            while (listElement->node != NULL) {
                RefactoringSon(listElement->node, node);
                listElement = listElement->next;
            }
            break;
        case Let:
            RefactoringSon(node->content.let.t3, node);
            RefactoringSon(node->content.let.t2, node);
            RefactoringSon(node->content.let.var, node);
            break;
        case FRic:
            RefactoringSon(node->content.fRic.t, node);
            RefactoringSon(node->content.fRic.var, node);
            listElement = node->content.fRic.arg->head;
            while (listElement->node != NULL) {
                RefactoringSon(listElement->node, node);
                listElement = listElement->next;
            }
            break;
        case GCoRic:
            RefactoringSon(node->content.gCoRic.t, node);
            RefactoringSon(node->content.gCoRic.var, node);
            listElement = node->content.gCoRic.arg->head;
            while (listElement->node != NULL) {
                RefactoringSon(listElement->node, node);
                listElement = listElement->next;
            }
            break;
        case Constructor:
            listElement = node->content.jCostr.arg->head;
            while (listElement->node != NULL) {
                RefactoringSon(listElement->node, node);
                listElement = listElement->next;
            }
            break;
        case Constant:
            RefactoringSon(node->content.constant.var, node);
            break;
    }

    free(node);
}

void RefactoringNode(Node *oldNode, Node *newNode) {
    newNode->root = oldNode->root;
    newNode->reachable = oldNode->reachable;
//    assert(oldNode->building == False);
//    assert(oldNode->canonic == NULL);
//    assert(oldNode->visited == False);

    ListItem *parent = oldNode->parentNodes->head;
    while (parent->node != NULL) {
        //aggiorno il nuovo figlio nel genitore
        UpdateSon(oldNode, newNode, parent->node);
        newNode->rc++;
        //aggiorno i padri del nuovo nodo preservando i vecchi parent
        PushToListHT(newNode->parentNodes, parent->node);
        parent = parent->next;
    }
    FreeRC(oldNode);
}

Node *WeakCbVEval(Node *n) {//riduzione e aggiornamento archi Padre-Figlio
    printf("Eval Node, Label= %d\n", n->label);
    ListItem *listElement;
    Node *n1;
    Node *n2;
    Node *nodeReturn;
    switch (n->label) {
        case FVar:
            return n;
        case BVar:
            return n;
        case Piai:
            return n;
        case Shared:
            nodeReturn = n->content.shared.body;
            RefactoringNode(n, nodeReturn);
            return nodeReturn;
        case App:
            n2 = WeakCbVEval(n->content.app.right);
            n1 = WeakCbVEval(n->content.app.left);
            if (n1->label == Lam) {
                nodeReturn = WeakCbVEval(Inst(n1->content.lam.body, n1, n2));
                RefactoringNode(n, nodeReturn);
                return nodeReturn;
            } else
                return n;
        case Lam:
            return n;
        case Match:
            switch (n->content.match.body->label) {
                case Constructor://j
                    listElement = n->content.match.branches->head;
                    for (int i = 0; i < n->content.match.body->content.jCostr.n - 1; ++i) {
                        listElement = listElement->next;
                    }
                    //applico il ramo n-esimo agli argomenti del costruttore
                    n2 = AppReplacement(listElement->node, n->content.match.body->content.jCostr.arg);
                    nodeReturn = WeakCbVEval(n2);
                    RefactoringNode(n, nodeReturn);
                    return nodeReturn;
                case GCoRic://jDelta-c
                    n2 = InitGCoRic(n->content.match.body->content.gCoRic.var, n->content.match.body->content.gCoRic.t,
                                    0, InitListHT());
                    n1 = Inst(n->content.match.body->content.gCoRic.t, n->content.match.body->content.gCoRic.var, n2);
                    if (!(n1->label == Shared && n1->content.shared.body == n2))
                        FreeRC(n2);
                    n2 = AppReplacement(n1, n->content.match.body->content.gCoRic.arg);
                    RefactoringNode(n->content.match.body, n2);//todo
                    n->content.match.body = n2;
                    nodeReturn = WeakCbVEval(n);
                    return nodeReturn;
                default:
                    return n;
            }

        case Let:
            n->content.let.var->content.bvar.binder = WeakCbVEval(
                    n->content.let.t2);//todo esempio grafico binder non valuta sotto quindi? Modifico regola?
            n->content.let.var->content.bvar.binder->rc++;
            nodeReturn = WeakCbVEval(n->content.let.t3);
            RefactoringNode(n, nodeReturn);
            return nodeReturn;
        case FRic:
            listElement = n->content.fRic.arg->head;
            for (int i = 0; i < n->content.fRic.n - 1; ++i) {
                listElement = listElement->next;
            }
            //valuto l'arg n-esimo
            listElement->node = WeakCbVEval(listElement->node);
            if (listElement->node->label == Constructor) {
                n1 = Inst(n->content.fRic.t, n->content.fRic.var, n);//b
                nodeReturn = WeakCbVEval(AppReplacement(n1, n->content.fRic.arg));
                RefactoringNode(n, nodeReturn);
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
            RefactoringNode(n, nodeReturn);
            return nodeReturn;
    }
}

/***************************************---BuildEquivalenceClass---**************************************/
void PushNeighbour(Node *m, Node *c) {
    Node *m1 = WeakCbVEval(m);
    print_graph(nodesHT, m1);
    Node *c1 = WeakCbVEval(c);
    print_graph(nodesHT, c1);

    PushToListHT(m1->neighbour, c1);
    PushToListHT(c1->neighbour, m1);
    print_graph(nodesHT, NULL);
}


void SearchIfReachable(Node *n) {
    n->reachable = n->root;
    // visit parents
    assert(n->parentNodes != NULL);
    ListItem *m = n->parentNodes->head;
    while (m->node != NULL && n->reachable != True) {
        if (m->node->visited == False)
            SearchIfReachable(m->node);
        n->reachable = m->node->reachable;
        m = m->next;
    }
    n->visited = True;
}


void Propagate(Node *m, Node *c) {//propagazione nodi...
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
        case Piai:
            if (c->label == Piai)
                PushNeighbour(m->content.piai.body, c->content.piai.body);
            else
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
            if (c->label == Match && m->content.match.n == c->content.match.n) {
                PushNeighbour(m->content.match.body, c->content.match.body);
                ListItem *iter = m->content.match.branches->head;
                ListItem *iter2 = c->content.match.branches->head;
                for (int i = 0; i < m->content.match.n; ++i) {
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
            if (c->label == FRic && m->content.fRic.n == c->content.fRic.n) {
                PushNeighbour(m->content.fRic.t, c->content.fRic.t);
                ListItem *iter = m->content.fRic.arg->head;
                ListItem *iter2 = c->content.fRic.arg->head;
                for (int i = 0; i < m->content.fRic.n; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case GCoRic:
            if (c->label == GCoRic && m->content.gCoRic.n == c->content.gCoRic.n) {
                PushNeighbour(m->content.gCoRic.t, c->content.gCoRic.t);
                ListItem *iter = m->content.gCoRic.arg->head;
                ListItem *iter2 = c->content.gCoRic.arg->head;
                for (int i = 0; i < m->content.gCoRic.n; ++i) {
                    PushNeighbour(iter->node, iter2->node);
                    iter = iter->next;
                    iter2 = iter2->next;
                }
            } else
                PrintExit(3);
            break;
        case Constructor://j indica quale costruttore è
            if (c->label == Constructor && m->content.jCostr.j == c->content.jCostr.j &&
                m->content.jCostr.n == c->content.jCostr.n) {
                ListItem *iter = m->content.jCostr.arg->head;
                ListItem *iter2 = c->content.jCostr.arg->head;
                for (int i = 0; i < m->content.jCostr.n; ++i) {
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

void Enqueue(Node *n, Node *c) {
    if (n->canonic == NULL) {
        PushToListHT(c->queue, n);
        n->canonic = c;
    } else if (n->canonic != c)
        PrintExit(2);
}

void BuildClass(Node *c) {
    printf("BuildClass Node, Label= %d\n", c->label);
    c->building = True;
    c->queue = InitListHT();
    ListItem *iterQueue = c->queue->head;
    Enqueue(c, c);
    while (iterQueue->node != NULL) {
        Node *n = iterQueue->node;
        SearchIfReachable(n);
        if (n->reachable == False) {
            assert(n->neighbour->head->node == NULL);
            c->building = False;
            return;
        }
        // visit parents
        assert(n->parentNodes != NULL);
        ListItem *iterParents = n->parentNodes->head;
        while (iterParents->node != NULL) {
            if (iterParents->node->canonic == NULL)
                BuildClass(iterParents->node);
            else if (iterParents->node->canonic->building == True)
                PrintExit(2);
            iterParents = iterParents->next;
        }
        // visit neighbours
        assert(n->neighbour != NULL);
        ListItem *iterNeighbour = n->neighbour->head;
        while (iterNeighbour->node != NULL) {
            Enqueue(iterNeighbour->node, c);
            iterNeighbour = iterNeighbour->next;
        }

        if (n != c)
            Propagate(n, c);
        iterQueue = iterQueue->next;
    }
    c->building = False;
}


/*****************************************--- Main-Equivalence&Eval---**************************************/
void DAGCheckAndEval(List *nodesHT, Node *root1, Node *root2) {
    printf("START DAGCheckAndEval\n");
    root1->root = True;
    root2->root = True;
    print_graph(nodesHT, NULL);
    PushNeighbour(root1, root2);
    printf("START While\n");
    ListItem *nodes = nodesHT->head;
    while (nodes->node != NULL) {
        if (nodes->node->canonic == NULL)
            BuildClass(nodes->node);
        nodes = nodes->next;
    }
    printf("END DAGCheckAndEval\n");
}
