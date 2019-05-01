# AlphaLinearConversion
This project focuses on the realization of a graph visit algorithm.
This graph represents in the nodes distinct lambda terms according to a suitable coding in:
* Lam Nodes: Lambda abstraction
* App Nodes: Application
* Var Nodes: Variable

The graph provided as input to the algorithm will represent 2 graphs joined by a root node connected to the respective roots of the 2 graphs.
These graphs will be compared to show whether they represent the same lambda expression, evaluating in the complex case also under the lambda abstractions.
In the complex case we will use the Crumbling Abstract Machines by applying a process of crumbling the lambda terms.
