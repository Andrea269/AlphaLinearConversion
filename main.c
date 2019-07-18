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
    printf("END ------\n\n");
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
    printf("END ------\n\n");
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
    printf("END ------\n\n");
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
    printf("END ------\n\n");
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
    printf("END ------\n\n");
}

void TestLet() {
    printf("START TestLet\n\n");
    nodesHT = InitListHT();
    struct NodeType *type = malloc(sizeof(struct NodeType));

    struct Node *var = PushToNodes(InitBVar(NULL));
    struct Node *t2 = PushToNodes(InitFVar(NULL));
    struct Node *node1App = PushToNodes(InitApp(var, var));
    struct Node *nodeLet = PushToNodes(InitLet(var, type, t2, node1App));

    struct Node *var2 = PushToNodes(InitBVar(NULL));
    struct Node *node1App2 = PushToNodes(InitApp(var2, var2));
    struct Node *nodeLet2 = PushToNodes(InitLet(var2, type, t2, node1App2));

    struct Node *r1=WeakCbVEval(nodeLet);
    struct Node *r2=WeakCbVEval(nodeLet2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}

void TestFunRicJTHConstructor() {
    printf("START TestLet\n\n");
    nodesHT = InitListHT();

    struct Node *var = PushToNodes(InitBVar(NULL));
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = PushToNodes(InitFVar(NULL));
    struct Node *node12Var = PushToNodes(InitFVar(NULL));
    struct Node *node13Var = PushToNodes(InitFVar(NULL));
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct ListHT *argF = InitListHT();
    struct Node *node1Var = PushToNodes(InitFVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *nodeConstructor = PushToNodes(InitConstructor(3, arg));
    PushToListHT(argF, node1Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = PushToNodes(InitApp(node1Var, node1Var));
    struct Node *nodeFunRic = PushToNodes(InitFRic(var, f, 3, argF));

    struct ListHT *arg2 = InitListHT();
    struct Node *node112Var = PushToNodes(InitFVar(NULL));
    struct Node *node122Var = PushToNodes(InitFVar(NULL));
    struct Node *node132Var = PushToNodes(InitFVar(NULL));
    PushToListHT(arg2, node112Var);
    PushToListHT(arg2, node122Var);
    PushToListHT(arg2, node132Var);
    struct ListHT *argF2 = InitListHT();
    struct Node *node212Var = PushToNodes(InitFVar(NULL));
    struct Node *node222Var = PushToNodes(InitFVar(NULL));
    struct Node *nodeConstructor2 = PushToNodes(InitConstructor(3, arg2));
    PushToListHT(argF2, node212Var);
    PushToListHT(argF2, node222Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = PushToNodes(InitApp(node1Var, node1Var));
    struct Node *nodeFunRic2 = PushToNodes(InitFRic(var, f2, 3, argF2));


    struct Node *r1=WeakCbVEval(nodeFunRic);
    struct Node *r2=WeakCbVEval(nodeFunRic2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestFunRicNoJTHConstructor() {
    printf("START TestLet\n\n");
    nodesHT = InitListHT();

    struct Node *var = PushToNodes(InitBVar(NULL));

    struct ListHT *argF = InitListHT();
    struct Node *node1Var = PushToNodes(InitFVar(NULL));
    struct Node *node2Var = PushToNodes(InitFVar(NULL));
    struct Node *node3Var = PushToNodes(InitFVar(NULL));
    PushToListHT(argF, node1Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, node3Var);
    struct Node *f = PushToNodes(InitApp(node1Var, node1Var));

    struct Node *nodeFunRic = PushToNodes(InitFRic(var, f, 3, argF));

    struct ListHT *argF2 = InitListHT();
    struct Node *node12Var = PushToNodes(InitFVar(NULL));
    struct Node *node22Var = PushToNodes(InitFVar(NULL));
    struct Node *node32Var = PushToNodes(InitFVar(NULL));
    PushToListHT(argF2, node12Var);
    PushToListHT(argF2, node22Var);
    PushToListHT(argF2, node32Var);
    struct Node *f2 = PushToNodes(InitApp(node1Var, node1Var));
    struct Node *nodeFunRic2 = PushToNodes(InitFRic(var, f2, 3, argF2));


    struct Node *r1=WeakCbVEval(nodeFunRic);
    struct Node *r2=WeakCbVEval(nodeFunRic2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
/***********************************---MAIN-TEST---***********************************
 * @return  0-> Successfully completed
 *          1-> ERROR: Exit in InitNode
 *          2-> ERROR: Exit in BuildClass
 *          3-> ERROR: Exit in Propagate
*/
int main() {
    printf("START MAIN\n\n");

//    TestAppCorrect();
//    TestMatchConstructor();
//    TestMatchCoRic();
//    TestLet();
//    TestFunRicJTHConstructor();
    TestFunRicNoJTHConstructor();



//    TestAppError();
//    TestMatchError();
    printf("\nEND MAIN\n");
    return 0;
}

//void BasicTest() {
//    printf("START Test\n\n");
//    nodesHT = InitListHT();
//
//    struct Node *r1=WeakCbVEval(NULL);
//    struct Node *r2=WeakCbVEval(NULL);
//    PushNeighbour(r1, r2);
//    DAGCheckAndEval(nodesHT);
//    printf("END ------\n\n");
//}
///**************************************************---PrintListHT---*************************************************/
//void PrintListHT() {//struct ListHT *listHT
//    printf("START PrintListHT\n");
//    int count = nodesHT->count;
//    printf("Numero nodi %d\n", count);
//    struct ListElement *nodes = nodesHT->head;
//
//    for (int i = 0; i < count; ++i) {
//        printf("PrintListNode - Nodes\n");
//        switch (nodes->node->label) {
//            case FVar:
//                printf("VarNode -Type Free Var\n");
//                break;
//            case BVar:
//                printf("VarNode -Type Binder Var\n");
//                break;
//            case Shared:
//                printf("VarNode -Type Shared\n");
//                break;
//            case App:
//                printf("AppNode -Type App\n");
//                printf("CRight -Type %d\n", nodes->node->content.app.right->label);
//                printf("CLeft -Type %d\n", nodes->node->content.app.left->label);
//                break;
//            case Lam:
//                printf("LamNode -Type Lam\n");
//                printf("CRight -Type %d\n", nodes->node->content.lam.body->label);
//                printf("CLeft -Type %d\n", nodes->node->content.lam.var->label);
//                struct Node *temp = nodes->node->content.lam.var->content.bvar.binder;
//                printf("binder -Type %d\n", temp->label);
//                break;
//            case Match:
//                printf("MatchNode -Type Match\n");
//                break;
//            case Let:
//                printf("LetNode -Type Let\n");
//                break;
//            case FRic:
//                printf("FRicNode -Type FRic\n");
//                break;
//            case GCoRic:
//                printf("GCoRicNode -Type GCoRic\n");
//                break;
//            case Constructor:
//                printf("ConstructorNode -Type Constructor\n");
//                break;
//            case Constant:
//                printf("ConstantNode -Type Constant\n");
//                break;
//        }
//        nodes = nodes->next;
//    }
//    printf("END PrintListHT\n\n");
//}