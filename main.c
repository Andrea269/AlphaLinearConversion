
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
int main() {
    printf("Hello, World!\n");
    return 0;
}