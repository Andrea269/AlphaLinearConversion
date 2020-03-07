//
// Created by andrea on 13/05/19.
//
#include "ConversionAlgorithm.c"

/**************************************************---TEST---*************************************************/
void TestAppCorrect() {
    printf("START TestAppCorrect\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);
    struct Node *node1BVar = InitBVar(NULL);
    struct Node *nodeApp1 = InitApp(node1BVar, node1BVar);
    struct Node *node1Lam = InitLam(node1BVar, nodeApp1);
    struct Node *node2BVar = InitBVar(NULL);
    struct Node *nodeApp2 = InitApp(node2BVar, node2BVar);
    struct Node *node11Lam = InitLam(node2BVar, nodeApp2);
    struct Node *node1App = InitApp(node1Lam, node1Var);
    struct Node *node11App = InitApp(node11Lam, node1Var);

    DAGCheckAndEval(nodesHT, node1App, node11App);
    printf("END ------\n\n");
}

void TestAppError() {
    printf("START TestAppError\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);
    struct Node *node2Var = InitFVarId(2);
    struct Node *node1App = InitApp(node1Var, node2Var);
    struct Node *node3Var = InitBVar(NULL);
    struct Node *node1Lam = InitLam(node3Var, node1App);
    struct Node *node11App = InitApp(node1Var, node3Var);
    struct Node *node4Var = InitBVar(NULL);
    struct Node *node11Lam = InitLam(node4Var, node11App);

    DAGCheckAndEval(nodesHT, node1Lam, node11Lam);
    printf("END ------\n\n");
}

void TestMatchConstructor() {
    printf("START TestMatchConstructor\n\n");
    nodesHT = InitListHT();
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(1);
    struct Node *node12Var = InitFVarId(2);
    struct Node *node13Var = InitFVarId(3);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitBVar(NULL);
    struct Node *node2Var = InitFVarId(4);
    struct Node *node3Var = InitFVarId(5);
    struct List *branches = InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct List *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = InitConstructor(2, arg, 3);
    struct Node *nodeMatchConst = InitMatch(nodeConst, branches, 3);
    struct Node *nodeConst2 = InitConstructor(2, arg, 3);
    struct Node *nodeMatchConst2 = InitMatch(nodeConst2, branches2, 3);

    DAGCheckAndEval(nodesHT, nodeMatchConst, nodeMatchConst2);
    printf("END ------\n\n");
}

void TestMatchCoRic() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();
    struct Node *node0Var = InitFVarId(1);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *nodeBVar1 = InitBVar(NULL);
    struct Node *nodeBVar2 = InitBVar(NULL);
    struct Node *node2Var = InitFVarId(5);
    struct Node *node3Var = InitFVarId(6);
    struct List *branches = InitListHT();
    struct Node *node1Lam = InitLam(nodeBVar1, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct List *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(nodeBVar2, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct Node *f = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric = InitGCoRic(var, f, 3, arg);
    struct Node *nodeMatchGcoric = InitMatch(nodeGcoric, branches, 3);
    struct Node *f2 = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric2 = InitGCoRic(var2, f2, 3, arg);
    struct Node *nodeMatchGcoric2 = InitMatch(nodeGcoric2, branches2, 3);

    DAGCheckAndEval(nodesHT, nodeMatchGcoric, nodeMatchGcoric2);
    printf("END ------\n\n");
}

void TestMatchError() {
    printf("START TestMatchError\n\n");
    nodesHT = InitListHT();
    struct Node *var = InitBVar(NULL);
    struct Node *node0Var = InitFVarId(1);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct Node *node1Var = InitBVar(NULL);
    struct Node *node2Var = InitFVarId(5);
    struct Node *node3Var = InitFVarId(6);
    struct List *branches = InitListHT();
    struct Node *node1Lam = InitLam(node1Var, node2Var);
    struct Node *node1App = InitApp(node1Lam, node2Var);
    struct Node *node2App = InitApp(node2Var, node2Var);
    struct Node *node3App = InitApp(node1Lam, node3Var);
    PushToListHT(branches, node1App);
    PushToListHT(branches, node2App);
    PushToListHT(branches, node3App);
    struct List *branches2 = InitListHT();
    struct Node *node1Lam2 = InitLam(node1Var, node2Var);
    struct Node *node1App2 = InitApp(node1Lam2, node2Var);
    struct Node *node2App2 = InitApp(node2Var, node2Var);
    struct Node *node3App2 = InitApp(node1Lam2, node3Var);
    PushToListHT(branches2, node1App2);
    PushToListHT(branches2, node2App2);
    PushToListHT(branches2, node3App2);
    struct Node *nodeConst = InitConstructor(3, arg, 3);
    struct Node *nodeMatchConst = InitMatch(nodeConst, branches, 3);
    struct Node *f = InitApp(node0Var, node0Var);
    struct Node *nodeGcoric2 = InitGCoRic(var, f, 3, arg);
    struct Node *nodeMatchGcoric2 = InitMatch(nodeGcoric2, branches2, 3);

    DAGCheckAndEval(nodesHT, nodeMatchConst, nodeMatchGcoric2);
    printf("END ------\n\n");
}

void TestLet() {
    printf("START TestLet\n\n");
    nodesHT = InitListHT();

    struct Node *var = InitBVar(NULL);
    struct Node *t2 = InitFVarId(1);
    struct Node *node1App = InitApp(var, var);
    struct Node *nodeLet = InitLet(var, t2, node1App);

    struct Node *var2 = InitBVar(NULL);
    struct Node *node1App2 = InitApp(var2, var2);
    struct Node *nodeLet2 = InitLet(var2, t2, node1App2);

    DAGCheckAndEval(nodesHT, nodeLet, nodeLet2);
    printf("END ------\n\n");
}

void TestFunRicJTHConstructor() {
    printf("START TestFunRicJTHConstructor\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    PushToListHT(arg2, node11Var);
    PushToListHT(arg2, node12Var);
    PushToListHT(arg2, node13Var);
    struct List *argF2 = InitListHT();
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var2, f2, 3, argF2);

    DAGCheckAndEval(nodesHT, nodeFunRic, nodeFunRic2);
    printf("END ------\n\n");
}

void TestFunRicNoJTHConstructor() {
    printf("START TestFunRicNoJTHConstructor\n\n");
    nodesHT = InitListHT();

    struct Node *var = InitBVar(NULL);
    struct Node *node1Var = InitFVarId(1);

    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(2);
    struct Node *node2Var = InitFVarId(3);
    struct Node *node3Var = InitFVarId(4);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, node3Var);
    struct Node *f = InitApp(node1Var, node1Var);

    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *argF2 = InitListHT();
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, node3Var);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var, f2, 3, argF2);

    DAGCheckAndEval(nodesHT, nodeFunRic, nodeFunRic2);
    printf("END ------\n\n");
}

void TestFunRicError() {
    printf("START TestFunRicError\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);
    struct Node *nodeBVar = InitBVar(NULL);

    struct Node *var = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    struct Node *node112Var = InitFVarId(7);
    struct Node *node122Var = InitFVarId(8);
    struct Node *node132Var = InitFVarId(9);
    PushToListHT(arg2, node112Var);
    PushToListHT(arg2, node122Var);
    PushToListHT(arg2, node132Var);
    struct List *argF2 = InitListHT();
    struct Node *node212Var = InitFVarId(10);
    struct Node *node222Var = InitFVarId(11);
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node212Var);
    PushToListHT(argF2, node222Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitLam(nodeBVar, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var, f2, 3, argF2);

    DAGCheckAndEval(nodesHT, nodeFunRic, nodeFunRic2);
    printf("END ------\n\n");
}


void TestFunRicJTHConstructorNoRoot() {
    printf("START TestFunRicJTHConstructorNoRoot\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    PushToListHT(arg2, node11Var);
    PushToListHT(arg2, node12Var);
    PushToListHT(arg2, node13Var);
    struct List *argF2 = InitListHT();
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var2, f2, 3, argF2);


    struct Node *varB = InitBVar(NULL);
    struct Node *varB2 = InitBVar(NULL);
    struct Node *nodeLam = InitLam(varB, nodeFunRic);
    struct Node *node2Lam = InitLam(varB2, nodeFunRic2);

    DAGCheckAndEval(nodesHT, nodeLam, node2Lam);
    printf("END ------\n\n");
}


void TestCoRicRoot() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();

    struct Node *node1Var = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    PushToListHT(arg2, node11Var);
    PushToListHT(arg2, node12Var);
    PushToListHT(arg2, node13Var);
    struct List *argF2 = InitListHT();
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var2, f2, 3, argF2);


    struct Node *varBG = InitBVar(NULL);
    struct List *argG = InitListHT();
    struct Node *nodeVarArg1 = InitFVarId(2);
    struct Node *nodeVarArg2 = InitFVarId(3);
    struct Node *nodeVarArg3 = InitFVarId(4);
    PushToListHT(argG, nodeVarArg1);
    PushToListHT(argG, nodeVarArg2);
    PushToListHT(argG, nodeVarArg3);


    struct Node *nodeGcoric = InitGCoRic(varBG, nodeFunRic, 3, argG);

    struct Node *nodeGcoric2 = InitGCoRic(varBG, nodeFunRic2, 3, argG);


    DAGCheckAndEval(nodesHT, nodeGcoric, nodeGcoric2);
    printf("END ------\n\n");
}


void TestCoRicNoRootReduce() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();

    struct Node *node1Var = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    PushToListHT(arg2, node11Var);
    PushToListHT(arg2, node12Var);
    PushToListHT(arg2, node13Var);
    struct List *argF2 = InitListHT();
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var2, f2, 3, argF2);


    struct Node *varBG = InitBVar(NULL);
    struct Node *varBG2 = InitBVar(NULL);
    struct List *argG = InitListHT();
    struct Node *nodeVarArg1 = InitFVarId(2);
    struct Node *nodeVarArg2 = InitFVarId(3);
    struct Node *nodeVarArg3 = InitFVarId(4);
    PushToListHT(argG, nodeVarArg1);
    PushToListHT(argG, nodeVarArg2);
    PushToListHT(argG, nodeVarArg3);
//    PushToListHT(argG, nodeFunRic);


    struct Node *nodeGcoric = InitGCoRic(varBG, nodeFunRic, 3, argG);

    struct Node *nodeGcoric2 = InitGCoRic(varBG2, nodeFunRic2, 3, argG);


    struct Node *nodeVar1 = InitFVarId(1);
    struct Node *node1BVar = InitBVar(NULL);
    struct Node *node1Lam = InitLam(node1BVar, nodeGcoric);
    struct Node *node2BVar = InitBVar(NULL);
    struct Node *node11Lam = InitLam(node2BVar, nodeGcoric2);
    struct Node *node1App = InitApp(node1Lam, nodeVar1);
    struct Node *node11App = InitApp(node11Lam, nodeVar1);


    DAGCheckAndEval(nodesHT, node1App, node11App);
    printf("END ------\n\n");
}


void TestCoRicNoRootNoReduce() {
    printf("START TestMatchCoRic\n\n");
    nodesHT = InitListHT();

    struct Node *node1Var = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);

    struct List *arg2 = InitListHT();
    PushToListHT(arg2, node11Var);
    PushToListHT(arg2, node12Var);
    PushToListHT(arg2, node13Var);
    struct List *argF2 = InitListHT();
    struct Node *nodeConstructor2 = InitConstructor(3, arg2, 3);
    PushToListHT(argF2, node0Var);
    PushToListHT(argF2, node2Var);
    PushToListHT(argF2, nodeConstructor2);
    struct Node *f2 = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic2 = InitFRic(var2, f2, 3, argF2);


    struct Node *varBG = InitBVar(NULL);
    struct List *argG = InitListHT();
    struct Node *nodeVarArg1 = InitFVarId(2);
    struct Node *nodeVarArg2 = InitFVarId(3);
    struct Node *nodeVarArg3 = InitFVarId(4);
    PushToListHT(argG, nodeVarArg1);
    PushToListHT(argG, nodeVarArg2);
    PushToListHT(argG, nodeVarArg3);


    struct Node *nodeGcoric = InitGCoRic(varBG, nodeFunRic, 3, argG);

    struct Node *nodeGcoric2 = InitGCoRic(varBG, nodeFunRic2, 3, argG);


    struct Node *nodeVar1 = InitFVarId(1);
    struct Node *node1BVar = InitBVar(NULL);
    struct Node *node1Lam = InitLam(node1BVar, nodeGcoric);
    struct Node *node2BVar = InitBVar(NULL);
    struct Node *node11Lam = InitLam(node2BVar, nodeGcoric2);
    struct Node *node1App = InitApp(nodeVar1, node1Lam);
    struct Node *node11App = InitApp(nodeVar1, node11Lam);


    DAGCheckAndEval(nodesHT, node1App, node11App);
    printf("END ------\n\n");
}

void Test() {
    printf("START Test\n\n");
    nodesHT = InitListHT();
    struct Node *node1Var = InitFVarId(1);
    struct Node *node1BVar = InitBVar(NULL);
    struct Node *nodeApp1 = InitApp(node1BVar, node1BVar);
    struct Node *node1Lam = InitLam(node1BVar, nodeApp1);
    struct Node *node2BVar = InitBVar(NULL);
    struct Node *nodeApp2 = InitApp(node2BVar, node2BVar);
    struct Node *nodeSH = InitShared(nodeApp2);
    struct Node *node11Lam = InitLam(node2BVar, nodeSH);
    struct Node *root1 = InitApp(node1Lam, node1Var);
    struct Node *root2 = InitApp(node11Lam, node1Var);

    DAGCheckAndEval(nodesHT, root1, root2);
    printf("END ------\n\n");
}

void TestFunRicJTHConstructorRid() {
    printf("START TestFunRicJTHConstructor\n\n");
    nodesHT = InitListHT();

    struct Node *node1Var = InitFVarId(1);
    struct Node *err = InitFVarId(1);

    struct Node *var = InitBVar(NULL);
//    struct Node *var2 = InitBVar(NULL);
    struct List *arg = InitListHT();
    struct Node *node11Var = InitFVarId(2);
    struct Node *node12Var = InitFVarId(3);
    struct Node *node13Var = InitFVarId(4);
    PushToListHT(arg, node11Var);
    PushToListHT(arg, node12Var);
    PushToListHT(arg, node13Var);
    struct List *argF = InitListHT();
    struct Node *node0Var = InitFVarId(5);
    struct Node *node2Var = InitFVarId(6);
    struct Node *nodeConstructor = InitConstructor(3, arg, 3);
    PushToListHT(argF, node0Var);
    PushToListHT(argF, node2Var);
    PushToListHT(argF, nodeConstructor);
    struct Node *f = InitApp(node1Var, node1Var);
    struct Node *nodeFunRic = InitFRic(var, f, 3, argF);
    struct Node *nodeFunRaaaic = InitFRic(var, f, 3, argF);

    struct Node *app1 = InitApp(node1Var, node1Var);
    struct Node *app2 = InitApp(app1, node0Var);
    struct Node *app3 = InitApp(app2, node2Var);
    struct Node *app4 = InitApp(app3, nodeConstructor);

    struct Node *nodeSH = InitShared(nodeFunRic);
    struct Node *nodeSH2 = InitShared(app4);
    DAGCheckAndEval(nodesHT, nodeSH, nodeSH2);

//    DAGCheckAndEval(nodesHT, nodeFunRic, app4);
    printf("END ------\n\n");
}
/***********************************---MAIN-TEST---***********************************
 * @return  0-> Successfully completed
 *          1-> ERROR: Exit in InitNode
 *          2-> ERROR: Exit in BuildClass
 *          3-> ERROR: Exit in Propagate
 *          4-> ERROR: Exit in BuildClass
 *          134 -> ERROR: Exit with assert(0) (interrupted by signal 6: SIGABRT)*/
int main() {
    printf("START MAIN\n\n");
    //Test sottostanti devono terminare con la fine del programma e non con errore

    mod=0;


    Test();
    TestAppCorrect();
    TestMatchConstructor();
    TestMatchCoRic();
    TestLet();
    TestFunRicJTHConstructor();
    TestFunRicNoJTHConstructor();

    TestFunRicJTHConstructorRid();




    //Test sottostanti è giusto che terminano con un errore, sarebbe errato altrimenti
//    TestFunRicError();
//    TestAppError();
//    TestMatchError();
    printf("\nEND MAIN\n");
    return 0;
}

/*
0	Fvar
1	BVar
2	Piai
3	Shared
4	App
5	Lam
6	Match
7	Let
8	FRic
9	GCoRic
10	Constructor
11	Constant
 */
