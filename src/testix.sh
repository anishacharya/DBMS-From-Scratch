#!/bin/bash - 

set -o nounset                              # Treat unset variables as an error

unzip $1.zip
cd $1
cd codebase
cd rbf
make clean && make
cd ../ix
make clean && make

./ixtest1
./ixtest2
./ixtest3
./ixtest4a
./ixtest4b
./ixtest5a
./ixtest5b
./ixtest6a
./ixtest6b
./ixtest7
./ixtest8
./ixtest9
./ixtest_extra_1
./ixtest_extra_2
./ixtest_extra_3

