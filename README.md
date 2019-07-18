# AlphaLinearConversion
This project focuses on the realization of a two-graph visit algorithm to determine if they represent the same lambda expression.

Each graph represents lambda terms in nodes, distinguished according to an appropriate coding in:
- Lam Nodes: Lambda abstraction
- App Nodes: Application
- Var Nodes: Variable

The algorithm is given a list of nodes, representing the 2 graphs to be evaluated connected in their roots by an indirect arc.
At this point the algorithm will terminate if the two graphs represent the same lambda expression as it will exit failing if it detects differences.
In the complex case it will also be evaluated under the lambda abstractions and the Crumbling Abstract Machines will be used by applying a process of crumbling the lambda terms.


gcc main.c -o main