/*
    Test: reduce terms containing Church numerals
    (https://en.wikipedia.org/wiki/Church_encoding)
 */

// Size of the term to test
#define N 10

#include "ConversionAlgorithm.c"

typedef struct Node* Node;

Node num(int n) {
    // Crea numerali di Church
    // num(n) == \ab. a^n(b)
    Node a = InitBVar(NULL);
    Node b = InitBVar(NULL);
    Node body = b;
    for(int i=0; i<n; i++) {
        body = InitApp(a, body);
    }
    Node absb = InitLam(b, body);
    Node absa = InitLam(a, absb);
    return absa;
}

Node mk_id() {
    // Crea identita' \x.x
    Node x = InitBVar(NULL);
    Node abs = InitLam(x, x);
    return abs;
}

Node mk_apps(Node head, Node* args) {
    // Crea un'applicazione iterata
    // Uso: mk_apps(head, (Node[]){arg1,arg2,...,NULL});
    while(*args) {
        Node arg = *args++;
        head = InitApp(head, arg);
    }
    return head;
}

Node mk_term(int n) {
    // Crea termine da testare
    Node noden = num(n);
    Node two = num(2);
    Node id = mk_id();
    return mk_apps(noden, (Node[]){two,id,id,NULL});
}

int main(){
    nodesHT = InitListHT();
    printf("START Test Church Numerals n=%d \n\n", N);
    Node m=mk_term(N);
    m->root=True;
    Node c=mk_id();
    c->root=True;
    PushNeighbour(m, c);
    DAGCheckAndEval(nodesHT);
    printf("END ------\n\n");
    return 0;
}
