#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_1(const string &indexFileName)
{
    // Functions tested
    // 1. Create Index File **
    // 2. Open Index File **
    // 3. Create Index File -- when index file is already created **
    // 4. Close Index File **
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 1****" << endl;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    // open index file
    IXFileHandle fileHandle;
    assertOpenIndexFile(success, indexManager, indexFileName, fileHandle);

    // create duplicate index file
    assertCreateIndexFile(fail, indexManager, indexFileName);

    // close index file
    assertCloseIndexFile(success, indexManager, fileHandle);

    return success;
}

int main()
{
    //Global Initializations
    indexManager = IndexManager::instance();

    const string indexFileName = "age_idx";

    testCase_1(indexFileName);
    cerr << "IX_Test Case 1 passed" << endl;
}

