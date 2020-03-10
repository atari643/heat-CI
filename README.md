HEAT
====================

Mathematical problem
---------------------

This C program aims at solving the following **heat propagation** equation

```math
\frac{\partial u(x,t) }{\partial t} - \Delta u(x,t) = 0 \qquad \forall  t \in [0,T] \, , \forall x \in [0,1]^2
```
```math
u(x,t) = 1 \, \qquad \forall  t \in [0,T] \, , \forall x \in \partial [0,1]^2.
```

Project
---------------------

This program serves as a toy code.
Several software engineering techniques are used:

* CMake build system with CTest
* Doxygen documentation
* A pipeline to test the code, either gitlab-ci (.gitlab-ci.yml) or Jenkins (Jenkinsfile) can be used
* Org-mode script for the code analysis and the integration into a SonarQube instance


Contributors
------------
EMPTY
