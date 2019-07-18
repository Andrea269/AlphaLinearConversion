//
// Created by andrea on 13/05/19.
//
#include "ConversionAlgorithm.c"
struct ListHT *nodesHT;//Node List
/*****************************---USEFUL FUNCTION---************************************/
struct Node *PushToNodes(struct Node *node){
    PushToListHT(nodesHT, node);
    return node;
};
/**************************************************---TEST---*************************************************/
void TestAppCorrect() {
    printf("START TestAppCorrect\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = PushToNodes(InitFVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node1App = PushToNodes(InitApp(node1Var, node2Var));
    struct Node *node3Var = PushToNodes(InitBVar(NULL));
    struct Node *node1Lam = PushToNodes(InitLam(node3Var, node1App));
    struct Node *node11App = PushToNodes(InitApp(node1Var, node2Var));
    struct Node *node4Var = PushToNodes(InitBVar(NULL));
    struct Node *node11Lam = PushToNodes(InitLam(node4Var, node11App));

    struct Node *r1=WeakCbVEval(node1Lam);
    struct Node *r2=WeakCbVEval(node11Lam);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("\nEND ------\n");
}
void TestAppError() {
    printf("START TestAppError\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = PushToNodes(InitFVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node1App = PushToNodes(InitApp(node1Var, node2Var));
    struct Node *node3Var = PushToNodes(InitBVar(NULL));
    struct Node *node1Lam = PushToNodes(InitLam(node3Var, node1App));
    struct Node *node11App = PushToNodes(InitApp(node1Var, node3Var));
    struct Node *node4Var = PushToNodes(InitBVar(NULL));
    struct Node *node11Lam = PushToNodes(InitLam(node4Var, node11App));

    struct Node *r1=WeakCbVEval(node1Lam);
    struct Node *r2=WeakCbVEval(node11Lam);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("\nEND ------\n");
}
void TestMatchConstructor() {
    printf("START TestMatchConstructor\n\n");
    nodesHT = InitListHT();
    struct NodeType *type = malloc(sizeof(struct NodeType));
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = PushToNodes(InitFVar(NULL));
    struct Node *node12Var = PushToNodes(InitFVar(NULL));
    struct Node *node13Var = PushToNodes(InitFVar(NULL));
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = PushToNodes(InitBVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node3Var = PushToNodes(InitFVar(NULL));
    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App = PushToNodes(InitApp(node1Lam, node2Var));
    struct Node *node2App = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App = PushToNodes(InitApp(node1Lam, node3Var));
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App2 = PushToNodes(InitApp(node1Lam2, node2Var));
    struct Node *node2App2 = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App2 = PushToNodes(InitApp(node1Lam2, node3Var));
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = PushToNodes(InitConstructor(3, arg));
    struct Node *nodeMatchConst = PushToNodes(InitMatch(type, type, nodeConst, branches));
    struct Node *nodeConst2 = PushToNodes(InitConstructor(3, arg));
    struct Node *nodeMatchConst2 = PushToNodes(InitMatch(type, type, nodeConst2, branches2));

    struct Node *r1=WeakCbVEval(nodeMatchConst);
    struct Node *r2=WeakCbVEval(nodeMatchConst2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("\nEND ------\n");
}
void TestMatchCoRic() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();
    struct NodeType *type = malloc(sizeof(struct NodeType));
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = PushToNodes(InitFVar(NULL));
    struct Node *node12Var = PushToNodes(InitFVar(NULL));
    struct Node *node13Var = PushToNodes(InitFVar(NULL));
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = PushToNodes(InitBVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node3Var = PushToNodes(InitFVar(NULL));
    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App = PushToNodes(InitApp(node1Lam, node2Var));
    struct Node *node2App = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App = PushToNodes(InitApp(node1Lam, node3Var));
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App2 = PushToNodes(InitApp(node1Lam2, node2Var));
    struct Node *node2App2 = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App2 = PushToNodes(InitApp(node1Lam2, node3Var));
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeGcoric=PushToNodes(InitGCoRic(type, 3, arg));
    struct Node *nodeMatchGcoric =PushToNodes( InitMatch( type, type, nodeGcoric, branches));
    struct Node *nodeGcoric2=PushToNodes(InitGCoRic(type, 3, arg));
    struct Node *nodeMatchGcoric2 = PushToNodes(InitMatch( type, type, nodeGcoric2, branches2));

    struct Node *r1=WeakCbVEval(nodeMatchGcoric);
    struct Node *r2=WeakCbVEval(nodeMatchGcoric2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("\nEND ------\n");
}
void TestMatchError() {
    printf("START TestMatchError\n\n");
    nodesHT = InitListHT();
    struct NodeType *type = malloc(sizeof(struct NodeType));

    struct ListHT *arg = InitListHT();
    struct Node *node11Var = PushToNodes(InitFVar(NULL));
    struct Node *node12Var = PushToNodes(InitFVar(NULL));
    struct Node *node13Var = PushToNodes(InitFVar(NULL));
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = PushToNodes(InitBVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node3Var = PushToNodes(InitFVar(NULL));
    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App = PushToNodes(InitApp(node1Lam, node2Var));
    struct Node *node2App = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App = PushToNodes(InitApp(node1Lam, node3Var));
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = PushToNodes(InitLam(node1Var, node2Var));
    struct Node *node1App2 = PushToNodes(InitApp(node1Lam2, node2Var));
    struct Node *node2App2 = PushToNodes(InitApp(node2Var, node2Var));
    struct Node *node3App2 = PushToNodes(InitApp(node1Lam2, node3Var));
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = PushToNodes(InitConstructor(3, arg));
    struct Node *nodeMatchConst = PushToNodes(InitMatch(type, type, nodeConst, branches));
    struct Node *nodeGcoric2=PushToNodes(InitGCoRic(type, 3, arg));
    struct Node *nodeMatchGcoric2 = PushToNodes(InitMatch( type, type, nodeGcoric2, branches2));

    struct Node *r1=WeakCbVEval(nodeMatchConst);
    struct Node *r2=WeakCbVEval(nodeMatchGcoric2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("\nEND ------\n");
}
/***********************************---MAIN-TEST---***********************************
 * @return  0-> Successfully completed
 *          1-> ERROR: Exit in InitNode
 *          2-> ERROR: Exit in BuildClass
 *          3-> ERROR: Exit in Propagate
*/
int main() {
    printf("START MAIN\n\n");

    TestAppCorrect();
    TestMatchConstructor();
    TestMatchCoRic();



//    TestAppError();
//    TestMatchError();
    printf("\nEND MAIN\n");
    return 0;
}



/**************************************************---PrintListHT---*************************************************/
void PrintListHT() {//struct ListHT *listHT
    printf("START PrintListHT\n");
    int count = nodesHT->count;
    printf("Numero nodi %d\n", count);
    struct ListElement *nodes = nodesHT->head;

    for (int i = 0; i < count; ++i) {
        printf("PrintListNode - Nodes\n");
        switch (nodes->node->label) {
            case FVar:
                printf("VarNode -Type Free Var\n");
                break;
            case BVar:
                printf("VarNode -Type Binder Var\n");
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
                struct Node *temp = nodes->node->content.lam.var->content.bvar.binder;
                printf("binder -Type %d\n", temp->label);
                break;
            case Match:
                printf("MatchNode -Type Match\n");
                break;
            case Let:
                printf("LetNode -Type Let\n");
                break;
            case FRic:
                printf("FRicNode -Type FRic\n");
                break;
            case GCoRic:
                printf("GCoRicNode -Type GCoRic\n");
                break;
            case Constructor:
                printf("ConstructorNode -Type Constructor\n");
                break;
            case Constant:
                printf("ConstantNode -Type Constant\n");
                break;
        }
        nodes = nodes->next;
    }
    printf("END PrintListHT\n\n");
}