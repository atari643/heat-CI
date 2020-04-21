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
sonar.links.homepage=https://gitlab-ce.iut.u-bordeaux.fr/lp-dagpi-devops/heat
sonar.links.scm=https://gitlab-ce.iut.u-bordeaux.fr/lp-dagpi-devops/heat.git
sonar.projectKey=heat
sonar.projectDescription=Solve the heat propagation equation
sonar.projectVersion=1.0
sonar.scm.disabled=false
sonar.scm.provider=git
sonar.sourceEncoding=UTF-8
sonar.sources=.
sonar.exclusions=build/CMakeFiles/**,coverage/**,*.xml
sonar.lang.patterns.c++=**/*.c,**/*.h
sonar.lang.patterns.c++=**/*.cpp,**/*.hpp
sonar.c.errorRecoveryEnabled=true
sonar.c.includeDirectories=/usr/lib/gcc/x86_64-linux-gnu/9/include, /usr/local/include, /usr/lib/gcc/x86_64-linux-gnu/9/include-fixed, /usr/include/x86_64-linux-gnu, /usr/include, ., include
sonar.c.gcc.charset=UTF-8
sonar.c.gcc.regex=(?<file>.*):(?<line>[0-9]+):[0-9]+:\\\x20warning:\\\x20(?<message>.*)\\\x20\\\[(?<id>.*)\\\]
sonar.c.gcc.reportPath=build/heat-build.log
sonar.c.clangsa.reportPath=build/analyzer_reports/*/*.plist
sonar.c.clangtidy.reportPath=build/clang-tidy-report
sonar.c.coverage.reportPath=heat-coverage.xml
sonar.c.cppcheck.reportPath=heat-cppcheck.xml
sonar.c.valgrind.reportPath=heat-valgrind.xml
sonar.c.drmemory.reportPath=drmemory/**/results.txt
EOF
