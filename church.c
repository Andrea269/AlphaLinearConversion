/*
    Test: reduce terms containing Church numerals
    (https://en.wikipedia.org/wiki/Church_encoding)
 */

// Size of the term to test
#define N 10

#include "ConversionAlgorithm.c"

typedef Node* PNode;

PNode num(int n) {
    // Crea numerali di Church
    // num(n) == \ab. a^n(b)
    PNode a = InitBVar(NULL);
    PNode b = InitBVar(NULL);
    PNode body = b;
    for(int i=0; i<n; i++) {
        body = InitApp(a, body);
    }
    PNode absb = InitLam(b, body);
    PNode absa = InitLam(a, absb);
    return absa;
}

PNode mk_id() {
    // Crea identita' \x.x
    PNode x = InitBVar(NULL);
    PNode abs = InitLam(x, x);
    return abs;
}

PNode mk_apps(PNode head, PNode* args) {
    // Crea un'applicazione iterata
    // Uso: mk_apps(head, (PNode[]){arg1,arg2,...,NULL});
    while(*args) {
        PNode arg = *args++;
        head = InitApp(head, arg);
    }
    return head;
}

PNode mk_term(int n) {
    // Crea termine da testare
    PNode noden = num(n);
    PNode two = num(2);
    PNode id = mk_id();
    return mk_apps(noden, (PNode[]){two,id,id,NULL});
}

int main(){
    nodesHT = InitListHT();
    printf("START Test Church Numerals n=%d \n\n", N);

    DAGCheckAndEval(nodesHT, mk_term(N), mk_id());
    printf("END ------\n\n");
    return 0;
}
