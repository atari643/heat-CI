HEAT
====================

Quality monitoring
------------------

### GitLab badges
[![pipeline status](https://gitlab-ce.iut.u-bordeaux.fr/pt4/heat/badges/master/pipeline.svg)](https://gitlab-ce.iut.u-bordeaux.fr/pt4/heat/commits/master)
[![coverage report](https://gitlab-ce.iut.u-bordeaux.fr/pt4/heat/badges/master/coverage.svg)](https://gitlab-ce.iut.u-bordeaux.fr/pt4/heat/commits/master)

### Coverity badge
[![coverity status](https://scan.coverity.com/projects/19229/badge.svg)](https://scan.coverity.com/projects/heat)

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

Install
---------------------

This program requires : Git, a C compiler (GNU gcc for example) and CMake to build and install.

Example on Docker image Ubuntu 20.04
```sh
docker run -it ubuntu:20.04

apt update -y
apt install -y build-essential cmake git

# clone the project through git+ssh if you have added your ssh public key on Gitlab
git clone git@gitlab-ce.iut.u-bordeaux.fr:pt4/heat.git
# or through https with login+password authentication
git clone https://gitlab-ce.iut.u-bordeaux.fr/pt4/heat.git

cd heat
mkdir build
cd build
cmake ..
make
make test
make install
```

Contributors
------------
Florent Pruvost florent.pruvost@inria.fr
Pierre Ramet pierre.ramet@inria.fr
