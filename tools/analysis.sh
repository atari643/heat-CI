#!/bin/bash

#lcov --directory . --capture --output-file heat.lcov
#lcov_cobertura.py heat.lcov --output heat-coverage.xml
gcovr -r . -x -o heat-coverage.xml

export DEFINITIONS=""
export CPPCHECK_INCLUDES="-I. -Iinclude"
export SOURCES_TO_EXCLUDE="-ibuild/CMakeFiles/"
export SOURCES_TO_ANALYZE="."

run-clang-tidy -checks='*' -header-filter=. -p build -j5 > clang-tidy-report

cppcheck -v -f --language=c --platform=unix64 --enable=all --xml --xml-version=2 --suppress=missingIncludeSystem ${DEFINITIONS} ${CPPCHECK_INCLUDES} ${SOURCES_TO_EXCLUDE} ${SOURCES_TO_ANALYZE} 2> heat-cppcheck.xml

rats -w 3 --xml ${SOURCES_TO_ANALYZE} > heat-rats.xml

bash -c 'find ${SOURCES_TO_ANALYZE} -regex ".*\.c\|.*\.h" | vera++ - -showrules -nodup |& vera++Report2checkstyleReport.perl > heat-vera.xml'

valgrind --xml=yes --xml-file=heat-valgrind.xml --memcheck:leak-check=full --show-reachable=yes "./build/heat_seq" "10" "10" "200" "0" "0"
#mpirun "-np" "4" valgrind --xml=yes --xml-file=heat-valgrind.xml --memcheck:leak-check=full --show-reachable=yes --suppressions=/usr/share/openmpi/openmpi-valgrind.supp --suppressions=tools/heat-valgrind.supp "./build/heat_par" "10" "10" "200" "2" "2" "0"

mkdir -p drmemory
${DRMEMORY}/drmemory -logdir drmemory -- ./build/heat_seq "10" "10" "200" "0" "0"

# Create the config for sonar-scanner
cat > sonar-project.properties << EOF
sonar.host.url=http://alee.iut.bx1:9000/
sonar.links.homepage=https://gitlab.inria.fr/sed-bso/heat
sonar.links.scm=https://gitlab.inria.fr/sed-bso/heat.git
sonar.projectKey=heat:gitlab:master
sonar.projectDescription=Solve the heat propagation equation
sonar.projectVersion=2.0
sonar.scm.disabled=false
sonar.scm.provider=git
sonar.sourceEncoding=UTF-8
sonar.sources=.
sonar.exclusions=build/CMakeFiles/**
sonar.language=c
sonar.c.errorRecoveryEnabled=true
sonar.c.includeDirectories=$(echo | gcc -E -Wp,-v - 2>&1 | grep "^ " | tr '\n' ',')/usr/lib/x86_64-linux-gnu/openmpi/include,include,.
sonar.c.compiler.parser=GCC
sonar.c.compiler.charset=UTF-8
sonar.c.compiler.regex=^(.*):(\\\d+):\\\d+: warning: (.*)\\\[(.*)\\\]$
sonar.c.compiler.reportPath=build/heat-build.lo
sonar.c.clangsa.reportPath=build/analyzer_reports/*/*.plist
sonar.c.clangtidy.reportPath=clang-tidy-report
sonar.c.coverage.reportPath=heat-coverage.xml
sonar.c.cppcheck.reportPath=heat-cppcheck.xml
sonar.c.rats.reportPath=heat-rats.xml
sonar.c.vera.reportPath=heat-vera.xml
sonar.c.valgrind.reportPath=heat-valgrind.xml
sonar.c.drmemory.reportPath=drmemory/**/results.txt
EOF
