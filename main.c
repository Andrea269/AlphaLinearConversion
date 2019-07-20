//
// Created by andrea on 13/05/19.
//
#include "ConversionAlgorithm.c"
/**************************************************---TEST---*************************************************/
void TestAppCorrect() {
    printf("START TestAppCorrect\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node1App = InitApp(node1Var, node2Var);
    struct Node *node3Var = InitBVar(NULL);
    struct Node *node1Lam = InitLam(node3Var, node1App);
    struct Node *node11App = InitApp(node1Var, node2Var);
    struct Node *node4Var = InitBVar(NULL);
    struct Node *node11Lam = InitLam(node4Var, node11App);

    struct Node *r1=WeakCbVEval(node1Lam);
    struct Node *r2=WeakCbVEval(node11Lam);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestAppError() {
    printf("START TestAppError\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node1App = InitApp(node1Var, node2Var);
    struct Node *node3Var = InitBVar(NULL);
    struct Node *node1Lam = InitLam(node3Var, node1App);
    struct Node *node11App = InitApp(node1Var, node3Var);
    struct Node *node4Var = InitBVar(NULL);
    struct Node *node11Lam = InitLam(node4Var, node11App);

    struct Node *r1=WeakCbVEval(node1Lam);
    struct Node *r2=WeakCbVEval(node11Lam);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestMatchConstructor() {
    printf("START TestMatchConstructor\n\n");
    nodesHT = InitListHT();
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitFVar(NULL);
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node13Var = InitFVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitBVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node3Var = InitFVar(NULL);
    struct ListHT *branches =  InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = InitConstructor(3, arg);
    struct Node *nodeMatchConst = InitMatch(nodeConst, branches);
    struct Node *nodeConst2 = InitConstructor(3, arg);
    struct Node *nodeMatchConst2 = InitMatch(nodeConst2, branches2);

    struct Node *r1=WeakCbVEval(nodeMatchConst);
    struct Node *r2=WeakCbVEval(nodeMatchConst2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestMatchCoRic() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();
    struct Node *var = InitBVar(NULL);
    struct Node *node0Var = InitFVar(NULL);
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitFVar(NULL);
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node13Var = InitFVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitBVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node3Var = InitFVar(NULL);
    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *f = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric=InitGCoRic(var, f, 3, arg);
    struct Node *nodeMatchGcoric = InitMatch(nodeGcoric, branches);
    struct Node *f2 = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric2=InitGCoRic(var, f2, 3, arg);
    struct Node *nodeMatchGcoric2 = InitMatch(nodeGcoric2, branches2);

    struct Node *r1=WeakCbVEval(nodeMatchGcoric);
    struct Node *r2=WeakCbVEval(nodeMatchGcoric2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestMatchError() {
    printf("START TestMatchError\n\n");
    nodesHT = InitListHT();
    struct Node *var = InitBVar(NULL);
    struct Node *node0Var = InitFVar(NULL);

    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitFVar(NULL);
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node13Var = InitFVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitBVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node3Var = InitFVar(NULL);
    struct ListHT *branches = InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct ListHT *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = InitConstructor(3, arg);
    struct Node *nodeMatchConst = InitMatch(nodeConst, branches);
    struct Node *f = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric2=InitGCoRic(var, f, 3, arg);
    struct Node *nodeMatchGcoric2 = InitMatch(nodeGcoric2, branches2);

    struct Node *r1=WeakCbVEval(nodeMatchConst);
    struct Node *r2=WeakCbVEval(nodeMatchGcoric2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}

void TestLet() {
    printf("START TestLet\n\n");
    nodesHT = InitListHT();

    struct Node *var = InitBVar(NULL);
    struct Node *t2 = InitFVar(NULL);
    struct Node *node1App = InitApp(var, var);
    struct Node *nodeLet = InitLet(var, t2, node1App);

    struct Node *var2 = InitBVar(NULL);
    struct Node *node1App2 = InitApp(var2, var2);
    struct Node *nodeLet2 = InitLet(var2, t2, node1App2);

    struct Node *r1=WeakCbVEval(nodeLet);
    struct Node *r2=WeakCbVEval(nodeLet2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}

void TestFunRicJTHConstructor() {
    printf("START TestFunRicJTHConstructor\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVar(NULL);

    struct Node *var = InitBVar(NULL);
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitFVar(NULL);
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node13Var = InitFVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct ListHT *argF = InitListHT();
    struct Node *node0Var = InitFVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *nodeConstructor = InitConstructor(3, arg);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct ListHT *arg2 = InitListHT();
    struct Node *node112Var = InitFVar(NULL);
    struct Node *node122Var = InitFVar(NULL);
    struct Node *node132Var = InitFVar(NULL);
    PushToListHT(arg2, node112Var);
    PushToListHT(arg2, node122Var);
    PushToListHT(arg2, node132Var);
    struct ListHT *argF2 = InitListHT();
    struct Node *node212Var = InitFVar(NULL);
    struct Node *node222Var = InitFVar(NULL);
    struct Node *nodeConstructor2 = InitConstructor(3, arg2);
    PushToListHT(argF2, node212Var);
    PushToListHT(argF2, node222Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var, f2, 3, argF2);


    struct Node *r1=WeakCbVEval(nodeFunRic);
    struct Node *r2=WeakCbVEval(nodeFunRic2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}
void TestFunRicNoJTHConstructor() {
    printf("START TestFunRicNoJTHConstructor\n\n");
    nodesHT = InitListHT();

    struct Node *var = InitBVar(NULL);
    struct Node *node1Var = InitFVar(NULL);

    struct ListHT *argF = InitListHT();
    struct Node *node0Var = InitFVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *node3Var = InitFVar(NULL);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, node3Var);
    struct Node *f = InitApp(node1Var, node1Var);

    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct ListHT *argF2 = InitListHT();
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node22Var = InitFVar(NULL);
    struct Node *node32Var = InitFVar(NULL);
    PushToListHT(argF2, node12Var);
    PushToListHT(argF2, node22Var);
    PushToListHT(argF2, node32Var);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var, f2, 3, argF2);


    struct Node *r1=WeakCbVEval(nodeFunRic);
    struct Node *r2=WeakCbVEval(nodeFunRic2);
    PushNeighbour(r1, r2);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
}

void TestFunRicError() {
    printf("START TestFunRicError\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVar(NULL);
    struct Node *nodeBVar = InitBVar(NULL);

    struct Node *var = InitBVar(NULL);
    struct ListHT *arg = InitListHT();
    struct Node *node11Var = InitFVar(NULL);
    struct Node *node12Var = InitFVar(NULL);
    struct Node *node13Var = InitFVar(NULL);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct ListHT *argF = InitListHT();
    struct Node *node0Var = InitFVar(NULL);
    struct Node *node2Var = InitFVar(NULL);
    struct Node *nodeConstructor = InitConstructor(3, arg);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct ListHT *arg2 = InitListHT();
    struct Node *node112Var = InitFVar(NULL);
    struct Node *node122Var = InitFVar(NULL);
    struct Node *node132Var = InitFVar(NULL);
    PushToListHT(arg2, node112Var);
    PushToListHT(arg2, node122Var);
    PushToListHT(arg2, node132Var);
    struct ListHT *argF2 = InitListHT();
    struct Node *node212Var = InitFVar(NULL);
    struct Node *node222Var = InitFVar(NULL);
    struct Node *nodeConstructor2 = InitConstructor(3, arg2);
    PushToListHT(argF2, node212Var);
    PushToListHT(argF2, node222Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitLam(nodeBVar, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var, f2, 3, argF2);


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

    //Test sottostanti devono terminare con la fine del programma e non con errore
    TestAppCorrect();
    TestMatchConstructor();
    TestMatchCoRic();
    TestLet();
    TestFunRicNoJTHConstructor();
    TestFunRicJTHConstructor();

    //Test sottostanti è giusto che terminano con un errore, sarebbe errato altrimenti
//    TestFunRicError();
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

/**************************************************---PrintListHT---*************************************************/
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