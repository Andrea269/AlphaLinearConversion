//
// Created by andrea on 17/07/19.
//
// graphviz come pacchetto
// gcc -o main main.c && ./main ; dot -Tpdf graph.dot > graph.pdf
// && evince graph.pdf
//
// gcc -o church church.c && ./church ; dot -Tpdf graph.dot > graph.pdf
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/********---DATA STRUCTURES---*************/


typedef struct Node Node;
typedef struct List List;
typedef struct ListItem ListItem;
typedef enum Bool Bool;

List *nodesHT;//Node List

int mod = 0;//0-->Solo test errore ... 1--> Test con pdf... 2-->1 +in dettegio
int numTest = 0;
enum TypeNode {
    FVar, BVar, PiGreco, Shared, App, Lam, Match, Let, FRic, GCoRic, Constructor, Constant
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
struct NodePiGreco {
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
        struct NodePiGreco piGreco;
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
    List *neighbourNodes;
    List *queue;
    ListItem *iterQueue;
    Bool root;
    Bool reachable;
    Bool visited;
    Bool reduced;
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
const char *PrintLabel(enum TypeNode l) {
    switch (l) {
        case FVar:
            return "v";
        case BVar:
            return "b";
        case PiGreco:
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

void PrintNode(FILE *f, Node *n) {
    fprintf(f, "%d [label=\"%s\" ordering=\"out\"];\n", (long) n, PrintLabel(n->label));
    ListItem *listElement = n->parentNodes->head;
    while (listElement->node != NULL) {
        fprintf(f, "%d -> %d;\n", (long) listElement->node, (long) n);
        listElement = listElement->next;
    }
    listElement = n->neighbourNodes->head;
    while (listElement->node != NULL) {
        fprintf(f, "%d -> %d [style=\"dotted\" constraint=\"false\"];\n", (long) n, (long) listElement->node);
        listElement = listElement->next;
    }
    if (n->label == BVar && n->content.bvar.binder != NULL)
        fprintf(f, "%d -> %d;\n", (long) n, (long) n->content.bvar.binder);

    if (n->root == True) fprintf(f, "%d [shape=\"square\"];\n", (long) n);
}

void PrintGraph(List *l, Node *focus) {
    fprintf(stderr, "----- new graph -----\n");
    FILE *f = fopen("graph.dot", "w");
    fprintf(f, "digraph G {\n");
    ListItem *listElement = l->head;
    while (listElement->node != NULL) {
        if (listElement->node->rc > 0 || listElement->node->root == True)
            PrintNode(f, listElement->node);
        listElement = listElement->next;
    }
    if (focus != NULL) fprintf(f, "%d [color=\"red\"];\n", (long) focus);
    fprintf(f, "}\n");
    fclose(f);
    system("dot -Tpdf graph.dot > graph.pdf");
    if (mod != 0)
        getchar();
}

/*****************************---USEFUL FUNCTIONS---************************************/

void ExitMessage(int status) {
    if (mod == 0 || mod == 2)
        printf("\nTest Numero== %d --------", numTest);
    if(status==0)
        printf("\nSuccess: The two input DAGs are bi-similar.\nExit code:%d\n", status);
    else if(status>100)
        printf("\nFail: Input error.\nExit code:%d\n", status);
    else
        printf("\nFail: The two input DAGs are not bi-similar.\nExit code:%d\n", status);
    if(status!=0)
        exit(status);
}

void PrintExit(int error, Node *node) {
    PrintGraph(nodesHT, node);
    ExitMessage(error);
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
    node->reduced = False;
    node->parentNodes = InitListHT();
    node->neighbourNodes = InitListHT();
    node->iterQueue = malloc(sizeof(ListItem));;
    node->iterQueue->node=NULL;
    node->iterQueue->next=NULL;
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

Node *InitPiGreco(Node *var, Node *body) {
    Node *node = malloc(sizeof(Node));
    node->label = PiGreco;
    node->content.piGreco.var = var;
    var->rc++;
    node->content.piGreco.body = body;
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
        ExitMessage(1);
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
        ExitMessage(1);
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
    if(var->label!=BVar)
        ExitMessage(1);
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
        ExitMessage(1);
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
        case PiGreco:
            n1 = InitPiGreco(InitBVar(NULL), n->content.piGreco.body);
            n->copy = n1;
            temp = n1->content.piGreco.body;
            n1->content.piGreco.body = Inst(temp, l, sub);
            if (temp != n1->content.piGreco.body) {
                PushToListHT(n1->content.piGreco.body->parentNodes, n1);
                n1->content.piGreco.body->rc++;
                RemoveHT(temp->parentNodes, n1);
                temp->rc--;
            }
            n->copy = NULL;
            return n1;
        case Shared:
            return Inst(n->content.shared.body, l, sub);
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

Node *AppReplacement(Node *tj, List *args, Node *oldNode, Node *oldNodeArg) {
    Node *result = tj;
    RemoveHT(tj->parentNodes, oldNode);
    ListItem *arg = args->head;
    while (arg->node != NULL) {
        result = InitApp(result, arg->node);
        RemoveHT(arg->node->parentNodes, oldNodeArg);
        arg = arg->next;
    }
    return result;
}

void UpdateSon(Node *oldSon, Node *newSon) {
    ListItem *listElement = oldSon->parentNodes->head;
    while (listElement->node != NULL) {
        //aggiorno il nuovo figlio nel genitore
        int find = 0;
        switch (listElement->node->label) {
            case FVar:
                ExitMessage(999);
            case BVar:
                if (listElement->node->content.bvar.binder == oldSon)
                    listElement->node->content.bvar.binder = newSon;
                else
                    ExitMessage(999);
                break;
            case PiGreco:
                if (listElement->node->content.piGreco.body == oldSon)
                    listElement->node->content.piGreco.body = newSon;
                else if (listElement->node->content.piGreco.var == oldSon)
                    listElement->node->content.piGreco.var = newSon;
                else
                    ExitMessage(999);
                break;
            case Shared:
                if (listElement->node->content.shared.body == oldSon)
                    listElement->node->content.shared.body = newSon;
                else
                    ExitMessage(999);
                break;
            case App:
                if (listElement->node->content.app.left == oldSon)
                    listElement->node->content.app.left = newSon;
                else if (listElement->node->content.app.right == oldSon)
                    listElement->node->content.app.right = newSon;
                else
                    ExitMessage(999);
                break;
            case Lam:
                if (listElement->node->content.lam.body == oldSon)
                    listElement->node->content.lam.body = newSon;
                else if (listElement->node->content.lam.var == oldSon)
                    listElement->node->content.lam.var = newSon;
                else
                    ExitMessage(999);
                break;
            case Match:
                if (listElement->node->content.match.body == oldSon)
                    listElement->node->content.match.body = newSon;
                else {
                    assert(listElement->node->content.match.branches != NULL);
                    ListItem *branches = listElement->node->content.match.branches->head;
                    while (find == 0 && branches->node != NULL) {
                        if (branches->node == oldSon) {
                            branches->node = newSon;
                            find = 1;
                        }
                        branches = branches->next;
                    }
                    if (find == 0)
                        ExitMessage(999);
                }
                break;
            case Let:
                if (listElement->node->content.let.t3 == oldSon)
                    listElement->node->content.let.t3 = newSon;
                else if (listElement->node->content.let.t2 == oldSon)
                    listElement->node->content.let.t2 = newSon;
                else if (listElement->node->content.let.var == oldSon)
                    listElement->node->content.let.var = newSon;
                else
                    ExitMessage(999);
                break;
            case FRic:
                if (listElement->node->content.fRic.t == oldSon)
                    listElement->node->content.fRic.t = newSon;
                else if (listElement->node->content.fRic.var == oldSon)
                    listElement->node->content.fRic.var = newSon;
                else {
                    assert(listElement->node->content.fRic.arg != NULL);
                    ListItem *arg = listElement->node->content.fRic.arg->head;
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
                        ExitMessage(999);
                }
                break;
            case GCoRic:
                if (listElement->node->content.gCoRic.t == oldSon)
                    listElement->node->content.gCoRic.t = newSon;
                else if (listElement->node->content.gCoRic.var == oldSon)
                    listElement->node->content.gCoRic.var = newSon;
                else {
                    assert(listElement->node->content.gCoRic.arg != NULL);
                    ListItem *arg = listElement->node->content.gCoRic.arg->head;
                    while (arg->node != NULL) {
                        if (arg->node == oldSon) {
                            arg->node = newSon;
                            find = 1;
                        }
                        arg = arg->next;
                    }
                    if (find == 0)
                        ExitMessage(999);
                }
                break;
            case Constructor:
                assert(listElement->node->content.jCostr.arg != NULL);
                ListItem *arg = listElement->node->content.jCostr.arg->head;
                while (find == 0 && arg->node != NULL) {
                    if (arg->node == oldSon) {
                        arg->node = newSon;
                        find = 1;
                    }
                    arg = arg->next;
                }
                if (find == 0)
                    ExitMessage(999);
                break;
            case Constant:
                if (listElement->node->content.constant.var == oldSon)
                    listElement->node->content.constant.var = newSon;
                else
                    ExitMessage(999);
                break;
            default:
                ExitMessage(999);
        }
        newSon->rc++;
        //aggiorno i padri del nuovo nodo preservando i vecchi parent
        PushToListHT(newSon->parentNodes, listElement->node);
        listElement = listElement->next;
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

void RefactoringCanonic(List *listNeighbour, Node *oldCanonic, Node *newCanonic) {
    ListItem *listElement = listNeighbour->head;
    while (listElement->node != NULL) {
        if (listElement->node->canonic == oldCanonic) {
            listElement->node->canonic = newCanonic;
            RefactoringCanonic(listElement->node->neighbourNodes, oldCanonic, newCanonic);
        }
        listElement = listElement->next;
    }
}

void RefactoringNeighbour(Node *oldNode, Node *newNode) {
    newNode->neighbourNodes = oldNode->neighbourNodes;//todo ----------------------------------------
    ListItem *listElement = oldNode->neighbourNodes->head;
    while (listElement->node != NULL) {
        RemoveHT(listElement->node->neighbourNodes, oldNode);
        PushToListHT(listElement->node->neighbourNodes, newNode);
        if (listElement->node->canonic == oldNode) {
            listElement->node->canonic = newNode;
            RefactoringCanonic(listElement->node->neighbourNodes, oldNode, newNode);
        }
        listElement = listElement->next;
    }
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
        case PiGreco:
            RefactoringSon(node->content.piGreco.var, node);
            RefactoringSon(node->content.piGreco.body, node);
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


    UpdateSon(oldNode, newNode);
    RefactoringNeighbour(oldNode, newNode);
    ListItem *elementList = malloc(sizeof(ListItem));
    elementList->node = NULL;
    elementList->next = NULL;
    oldNode->iterQueue->next=elementList;
    newNode->iterQueue=oldNode->iterQueue;
    FreeRC(oldNode);
}


Node *WeakCbVEval(Node *n) {//riduzione e aggiornamento archi Padre-Figlio
    if (mod == 2) {
        printf("Eval Node, Label= %d\n", n->label);
        PrintGraph(nodesHT, n);
    }
//    n->reduced = True;
    ListItem *listElement;
    Node *n1;
    Node *n2;
    Node *nodeReturn = n;
    switch (n->label) {
        case FVar:
            break;
        case BVar:
            break;
        case PiGreco:
            break;
        case Shared:
            nodeReturn = n->content.shared.body;
            RemoveHT(nodeReturn->parentNodes, n);
            RefactoringNode(n, nodeReturn);
            break;
        case App:
            n2 = WeakCbVEval(n->content.app.right);
            n1 = WeakCbVEval(n->content.app.left);
            if (n1->label == Lam) {
                nodeReturn = WeakCbVEval(Inst(n1->content.lam.body, n1, n2));
                RefactoringNode(n, nodeReturn);
            } else {
                n->content.app.left = n1;
                n->content.app.right = n2;
            }
            break;
        case Lam:
            break;
        case Match:
            switch (n->content.match.body->label) {
                case Constructor://j
                    listElement = n->content.match.branches->head;
                    for (int i = 0; i < n->content.match.body->content.jCostr.n - 1; ++i) {
                        listElement = listElement->next;
                    }
                    //applico il ramo n-esimo agli argomenti del costruttore
                    n2 = AppReplacement(listElement->node, n->content.match.body->content.jCostr.arg, n, n->content.match.body);
                    nodeReturn = WeakCbVEval(n2);
                    RefactoringNode(n, nodeReturn);
                    break;
                case GCoRic://jDelta-c
                    n2 = InitGCoRic(n->content.match.body->content.gCoRic.var, n->content.match.body->content.gCoRic.t,
                                    0, InitListHT());
                    n1 = Inst(n->content.match.body->content.gCoRic.t, n->content.match.body->content.gCoRic.var, n2);
                    if (!(n1->label == Shared && n1->content.shared.body == n2))
                        FreeRC(n2);
                    n2 = AppReplacement(n1, n->content.match.body->content.gCoRic.arg, n->content.match.body, n->content.match.body);
                    RefactoringNode(n->content.match.body, n2);
                    n->content.match.body = n2;
                    nodeReturn = WeakCbVEval(n);
                    break;
                default:
                    break;
            }
            break;
        case Let:
            nodeReturn = WeakCbVEval(n->content.let.t2);
            RefactoringNode(n->content.let.var, nodeReturn);
            nodeReturn = WeakCbVEval(n->content.let.t3);
            RefactoringNode(n, nodeReturn);
            break;
        case FRic:
            listElement = n->content.fRic.arg->head;
            for (int i = 0; i < n->content.fRic.n - 1; ++i) {
                listElement = listElement->next;
            }
            //valuto l'arg n-esimo
            listElement->node = WeakCbVEval(listElement->node);
            if (listElement->node->label == Constructor) {
                n1 = Inst(n->content.fRic.t, n->content.fRic.var, n);//b
                nodeReturn = WeakCbVEval(AppReplacement(n1, n->content.fRic.arg, n, n));
                RefactoringNode(n, nodeReturn);
            } else {
                n->content.fRic.t = WeakCbVEval(n->content.fRic.t);
                nodeReturn = n;
            }
            break;
        case GCoRic:
            break;
        case Constructor:
            break;
        case Constant:
            nodeReturn = n->content.constant.var;
            RemoveHT(nodeReturn->parentNodes, n);
            RefactoringNode(n, nodeReturn);
            break;
    }
    return nodeReturn;
}

/***************************************---BuildEquivalenceClass---**************************************/
void PushNeighbour(Node *m, Node *c) {
    Node *m1 = WeakCbVEval(m);
    if (mod == 2)
        PrintGraph(nodesHT, m1);
    Node *c1 = WeakCbVEval(c);
    if (mod == 2)
        PrintGraph(nodesHT, c1);

    PushToListHT(m1->neighbourNodes, c1);
    PushToListHT(c1->neighbourNodes, m1);

    if (mod == 1 || mod == 2)
        PrintGraph(nodesHT, NULL);
}


void SearchIfReachable(Node *r) {
    r->reachable = r->root;
    // visit parents
    assert(r->parentNodes != NULL);
    ListItem *m = r->parentNodes->head;
    while (m->node != NULL && r->reachable != True) {
        if (m->node->visited == False)
            SearchIfReachable(m->node);
        r->reachable = m->node->reachable;
        m = m->next;
    }
    r->visited = True;
}


void Propagate(Node *m, Node *c) {//propagazione nodi...
    switch (m->label) {
        case FVar:
            if (m != c)
                PrintExit(3, m);
            break;
        case BVar:
            if (c->label == BVar) {
                assert(m->content.bvar.binder != NULL);
                assert(c->content.bvar.binder != NULL);
                if (m->content.bvar.binder->canonic != c->content.bvar.binder->canonic)
                    PrintExit(3, m);
            } else
                PrintExit(3, m);
            break;
        case PiGreco:
            if (c->label == PiGreco)
                PushNeighbour(m->content.piGreco.body, c->content.piGreco.body);
            else
                PrintExit(3, m);
            break;
        case Shared://un nodo Shared non dovrebbe mai incorrere in questo punto del codice
            ExitMessage(999);
        case App:
            if (c->label == App) {
                PushNeighbour(m->content.app.left, c->content.app.left);
                PushNeighbour(m->content.app.right, c->content.app.right);
            } else
                PrintExit(3, c);
            break;
        case Lam:
            if (c->label == Lam)
                PushNeighbour(m->content.lam.body, c->content.lam.body);
            else
                PrintExit(3, m);
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
                PrintExit(3, m);
            break;
        case Let://un nodo LET non dovrebbe mai incorrere in questo punto del codice
            ExitMessage(999);
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
                PrintExit(3, m);
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
                PrintExit(3, m);
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
                PrintExit(3, m);
            break;
        case Constant://un nodo Constant non dovrebbe mai incorrere in questo punto del codice
            ExitMessage(999);
    }
}

void Enqueue(Node *n, Node *c) {
    if (n->canonic == NULL) {
        PushToListHT(c->queue, n);
        n->canonic = c;
    } else if (n->canonic != c && c->canonic!=n) {
//        PrintExit(2, c);
        PrintExit(2, n->canonic);
    }
}

void BuildClass(Node *c) {
    if (mod == 2) {
        PrintGraph(nodesHT, c);
    }
    c->building = True;
    c->queue = InitListHT();
    c->iterQueue = c->queue->head;
    Enqueue(c, c);
    while (c->iterQueue->node != NULL) {
        Node *n = c->iterQueue->node;
        SearchIfReachable(n);
        if (n->reachable == False) {
            assert(n->neighbourNodes->head->node == NULL);
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
                PrintExit(4, n);
            else{
                printf("iterParents->node->canonic, Label= %d\n", iterParents->node->canonic->label);

                printf("iterParents->node, Label= %d\n", iterParents->node->label);
            }
            iterParents = iterParents->next;
        }
        // visit neighbours
        assert(n->neighbourNodes != NULL);
        ListItem *iterNeighbour = n->neighbourNodes->head;
        while (iterNeighbour->node != NULL) {
            Enqueue(iterNeighbour->node, c);
            iterNeighbour = iterNeighbour->next;
        }

        if (n != c) {
            c= WeakCbVEval(c);
            n= WeakCbVEval(n);

            Propagate(n, c);
        }
        c->iterQueue = c->iterQueue->next;
    }
    c->building = False;
}


/*****************************************--- Main-Equivalence&Eval---**************************************/
void DAGCheckAndEval(List *nodesHT, Node *root1, Node *root2) {
    printf("\nSTART DAGCheckAndEval\n");
    if (mod == 0 || mod == 2) {
        numTest++;
        printf("Test Numero== %d\n", numTest);
    }
    if (mod == 1 || mod == 2)
        PrintGraph(nodesHT, NULL);
    if (mod == 0 || mod == 2)
        printf("START While\n");

    root1->root = True;
    root2->root = True;


    PushNeighbour(root1, root2);
    ListItem *nodes = nodesHT->head;
    int n=0;
    while (nodes->node != NULL) {
        if (mod == 2)
            printf("n=%d\n", n++);
        if (nodes->node->canonic == NULL)
            BuildClass(nodes->node);
        nodes = nodes->next;
    }
    printf("END DAGCheckAndEval\n");

    ExitMessage(0);
}
