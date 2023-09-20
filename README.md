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
Several software engineering tools are used:

* CMake build system with CTest
* Gitlab-ci pipeline to test the code (.gitlab-ci.yml)
* Doxygen documentation
* Scripts for the code analysis and the integration into a SonarQube instance (tools/)

Install
---------------------

This program requires : __Git__, a __C compiler__ (GNU gcc for example) and __CMake__ to build and install. It
optionally depends on __doxygen__ to build the documentation and __MPI__ to build the parallel version of the
executable.

Example on Ubuntu 22.04
```sh
docker run -it ubuntu:22.04

apt update -y
apt install -y git build-essential cmake make doxygen libopenmpi-dev

# clone the project through git+ssh if you have added your ssh public key on Gitlab
git clone git@gitlab-ce.iut.u-bordeaux.fr:lp-dagpi-devops/heat.git
# or through https with login+password authentication
git clone https://gitlab-ce.iut.u-bordeaux.fr/lp-dagpi-devops/heat.git

cd heat
cmake -B build -S . -DCMAKE_INSTALL_PREFIX=$PWD/install -DBUILD_SHARED_LIBS=ON -DHEAT_USE_MPI=ON
cmake --build build
cmake --install build
cd build && ctest
```

Contributors
------------
Florent Pruvost florent.pruvost@inria.fr
