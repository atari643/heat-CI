pipeline {
    agent {
        docker {
            image 'hpclib/hiepacs'
        }
    }
    stages {
        stage('Checkout') {
            steps {
                git 'https://gitlab.inria.fr/sed-bso/heat.git'
                lastChanges format:'SIDE',matching: 'LINE'
            }
        }
        stage('Build') {
            steps {
                sh 'cmake . -DHEAT_USE_MPI=ON -DCMAKE_VERBOSE_MAKEFILE=ON -DCMAKE_C_FLAGS="-g -fPIC -fdiagnostics-show-option --coverage -fno-inline -Wall" -DCMAKE_EXE_LINKER_FLAGS="--coverage" -DCMAKE_INSTALL_PREFIX=$PWD/install'
                sh 'make |tee heat-build.log'
                sh 'make install'
            }
        }
        stage('Test') {
            steps {
                sh 'make test'
            }
        }
        stage('Sonar') {
            steps {
                sh 'lcov --directory . --capture --output-file heat.lcov'
                sh 'genhtml -o coverage heat.lcov'
                sh 'lcov_cobertura.py heat.lcov --output heat-coverage.xml'
                sh 'cppcheck -v -f --language=c --platform=unix64 --enable=all --xml --xml-version=2 --suppress=missingIncludeSystem -iCMakeFiles/ . 2> heat-cppcheck.xml'
                sh 'rats -w 3 --xml . > heat-rats.xml'
                sh 'mpirun -np 4 valgrind --xml=yes --xml-file=heat-valgrind.xml --memcheck:leak-check=full --show-reachable=yes --suppressions=/usr/share/openmpi/openmpi-valgrind.supp --suppressions=tools/heat-valgrind.supp ./heat_par 10 10 200 2 2 0'
                sh '''cat > sonar-project.properties << EOF
                sonar.host.url=https://hpclib-sed.bordeaux.inria.fr/sonarqube-dev
                sonar.links.homepage=https://gitlab.inria.fr/sed-bso/heat
                sonar.links.scm=https://gitlab.inria.fr/sed-bso/heat.git
                sonar.projectKey=heat
                sonar.projectName=Heat
                sonar.projectDescription=Solve the heat propagation equation
                sonar.projectVersion=gitlab
                sonar.branch=master
                sonar.login=c1416c617418f890576a747223ac9f6cd86353d6
                sonar.scm.disabled=false
                sonar.scm.provider=git
                sonar.sourceEncoding=UTF-8
                sonar.sources=.
                sonar.exclusions=CMakeFiles/**
                sonar.language=c++
                sonar.cxx.errorRecoveryEnabled=true
                sonar.cxx.compiler.parser=GCC
                sonar.cxx.includeDirectories=$(echo | gcc -E -Wp,-v - 2>&1 | grep "^ " | tr '\n')
                sonar.cxx.compiler.charset=UTF-8
                sonar.cxx.compiler.regex=^(.*):(\\d+):\\d+: warning: (.*)\\[(.*)\\]$
                sonar.cxx.compiler.reportPath=heat-build.log
                sonar.cxx.coverage.reportPath=heat-coverage.xml
                sonar.cxx.cppcheck.reportPath=heat-cppcheck.xml
                sonar.cxx.rats.reportPath=heat-rats.xml
                sonar.cxx.valgrind.reportPath=heat-valgrind.xml
                EOF'''
                sh 'sonar-scanner > sonar.log'
            }
        }
    }
}
