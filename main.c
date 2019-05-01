
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

int main() {
    printf("Hello, World!\n");
    return 0;
}