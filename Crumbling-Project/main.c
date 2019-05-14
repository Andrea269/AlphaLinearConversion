#include <stdio.h>
//
// Created by andrea on 08/05/19.
//
#include <stdio.h>
#include <stdlib.h>

enum Bool {
    True, False
};

struct List;
struct Term;
/*********************************************************************************/

struct Var {
    int dummy;
};
struct Node {
    struct Term *content;
    enum Bool copying;
    struct List *prev;//option
};
struct Env {
    struct Node *node1;
    struct Node *node2;
    struct List *env;//option
};
struct Crumblep {
    struct Term *term;
    struct Env *env;
};

/*********************************************************************************/
struct NodeVar {
    struct Var *var;
};
struct NodeApp {
    struct Term *term1;
    struct Term *term2;
};
struct NodeLam {
    struct NodeVar *var;
    struct Crumblep *crumblep;
};
struct NodeIf {
    struct Term *term;
    struct Crumblep *crumblepThen;
    struct Crumblep *crumblepElse;
};
struct NodeShared {
    struct Node *node;
};

enum TypeTerm {
    Bool, NodeVar, NodeApp, NodeLam, NodeIf, NodeShared
};
struct Term {
    enum TypeTerm label;
    union {
        enum Bool bool;
        struct NodeVar *var;
        struct NodeApp *app;
        struct NodeLam *lam;
        struct NodeIf *nodeIf;
        struct NodeShared *shared;
    } content;
};
/*********************************************************************************/
enum TypeElementList {
    Term, Node, Env
};
struct ElementList{
    enum TypeElementList  label;
    union {
        struct Term *term;
        struct Node *node;
        struct Env *env;
    } content;
};

struct List {
    struct List *head;
    struct ElementList *element;
    struct List *next;
    int count;
};

/**
 * Inizializza l'elemento in coda alla lista passata in input
 * @param listNode puntatore alla coda della lista
 * @param head della lista
 */
void InitList(struct List *listNode, struct List *head) {
    listNode->head = head;
    listNode->element = malloc(sizeof(struct ElementList));
    listNode->next = malloc(sizeof(struct List));
    listNode->count = 0;
};

struct List *PushToList(struct List *list, struct ElementList *prtElement) {
    list->element = prtElement;
    InitList(list->next, list->head);
    list->next->count = list->count + 1;
    return list->next;
};

void PushListNode(struct Node *node, struct Node *prev) {
    struct ElementList *element=malloc(sizeof(struct ElementList));
    element->label=Node;
    element->content.node=malloc(sizeof(struct Node));
    element->content.node=prev;
    node->prev = PushToList(node->prev, element);
}
/*********************************************************************************/

struct Node *mk_node(struct Term *content) {
    struct Node *node = malloc(sizeof(struct Node));
    node->content = content;
    node->copying = False;
    node->prev = NULL;
    return node;
}



struct Node *at(struct Node *node, struct Term *content, struct Env *env) {
    node->content = content;
    if (env == NULL)
        return node;
    else if (env->node1 != NULL && env->node2 != NULL) {
        PushListNode(env->node1, node);
        return env->node2;
    }
    exit(1);
}

struct Node *CopyingNode(struct Node *node, struct Node *cNode, struct Node *(f)()) {
    struct Term *saved = node->content;
    struct Term *shared = malloc(sizeof(struct Term));
    shared->content.shared->node = cNode;
    node->content = shared;
    node->copying = True;
    struct Node *res = f();
    node->content = saved;
    node->copying = False;
    return res;
}

struct Term *Dummy() {
    struct Term *var = malloc(sizeof(struct Term));
    var->content.var->var->dummy = 0;
    return var;
}

struct Node *Iota(struct Crumblep e) {
    struct Node *star = mk_node(Dummy());
    return at(star, e.term, e.env);
}

/*********************************************************************************/
void evalOpen(struct Node *n) {

}






/*********************************************************************************/
int main() {
    printf("Hello, World!\n");











    return 0;
}























