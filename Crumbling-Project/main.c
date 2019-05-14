#include <stdio.h>
//
// Created by andrea on 08/05/19.
//
#include <stdio.h>
#include <stdlib.h>

struct ListNode;
struct InfoNode;
struct Term;

enum Bool {
    True, False
};
/*********************************************************************************/

struct Node {
    struct Term *content;
    enum Bool copying;
    struct Node *prev;
};
struct Env {
    struct Node *node1;
    struct Node *node2;
    struct ListNode *env;
};
struct Crumblep {
    struct Term *term;
    struct Env *env;
};
/*********************************************************************************/
struct NodeVar {
    struct InfoNode *infoNode;
    int dummy;
    struct NodeLam *binder;
};
struct NodeApp {
    struct Term *left;
    struct Term *right;
    struct InfoNode *infoNode;
};
struct NodeLam {
    struct NodeVar *left;
    struct Crumblep *right;
    struct InfoNode *infoNode;
};
struct NodeIf {
    struct Term *term;
    struct Crumblep *ifThen;
    struct Crumblep *ifElse;
    struct InfoNode *infoNode;
};
struct NodeShared {
    struct Node *node;
};

struct TypeNode {
    int label;
    union {
        struct NodeVar var;
        struct NodeApp app;
        struct NodeLam lam;
        struct NodeIf nodeIf;
        struct NodeShared shared;
    } content;
};
/*********************************************************************************/

struct TypeList {
    int label;
    union {
        struct TypeNode typeNode;
        struct Node node;
        struct Env env;
    } content;
};
struct ListNode {
    struct ListNode *head;
    struct TypeList *node;
    struct ListNode *next;
    int count;
};
struct IndirectEdge {
    struct TypeNode *left;
    struct TypeNode *right;
    int isAlive;//1 se è vivo 0 altrimenti
};
struct ListIndirectEdge {
    struct ListIndirectEdge *head;
    struct IndirectEdge *edge;
    struct ListIndirectEdge *next;
    int count;
};

struct InfoNode {
    struct ListNode *parentNodes;
    struct TypeNode *canonic;
    int isAlive;//1 se è vivo 0 altrimenti
    struct ListIndirectEdge *indirectEdge;
};


struct Term {
    int label;
    union {
        enum Bool bool;
        struct TypeNode node;
    } content;
};
/*********************************************************************************/


struct Node *mk_node(struct Term *content) {
    struct Node *node = malloc(sizeof(struct Node));
    node->content = content;
    node->copying = False;
    node->prev = NULL;
    return node;
}

void evalOpen(struct Node *n) {

}

void push(struct Node *node, struct Node *prev) {
    node->prev = prev;
}

struct Node *at(struct Node *node, struct Term *content, struct Env *env) {
    node->content = content;
    if (env == NULL)
        return node;
    else if (env->node1 != NULL && env->node2 != NULL) {
        push(env->node1, node);
        return env->node2;
    }
    exit(0); //todo modificare
}

struct Node *CopyingNode(struct Node *node, struct Node *cNode, struct Node *(f)()) {
    struct Term *saved = node->content;
    struct Term *shared = malloc(sizeof(struct NodeShared));
    shared->content.node.content.shared.node = cNode;
    node->content = shared;
    node->copying = True;
    struct Node *res = f();
    node->content = saved;
    node->copying = False;
    return res;
}

struct Term *Dummy() {
    struct Term *var = malloc(sizeof(struct NodeVar));
    var->content.node.content.var.dummy = 0;
    var->content.node.content.var.binder = NULL;
    return var;
}

struct Node *Iota(struct Crumblep e) {
    struct Node *star = mk_node(Dummy());
    return at(star, e.term, e.env);
}

int main() {
    printf("Hello, World!\n");
    return 0;
}























