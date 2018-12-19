#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_3(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Destroy Index File **
    // 2. Open Index File -- should fail
    // 3. Scan  -- should fail
    cerr << endl << "****In Test Case 3****" << endl;

    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;

    // destroy index file
    assertDestroyIndexFile(success, indexManager, indexFileName);
    // open the destroyed index
    assertOpenIndexFile(fail, indexManager, indexFileName, ixfileHandle);

    // open scan
    assertInitalizeScan(fail, indexManager, ixfileHandle, attribute, NULL, NULL, true, true, ix_ScanIterator);

    return success;

}

int main()
{
    //Global Initializations
    indexManager = IndexManager::instance();

    const string indexFileName = "age_idx";
    Attribute attrAge;
    attrAge.length = 4;
    attrAge.name = "age";
    attrAge.type = TypeInt;

    RC result = testCase_3(indexFileName, attrAge);;
    if (result == success) {
        cerr << "IX_Test Case 3 passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 3 failed" << endl;
        return fail;
    }

}
