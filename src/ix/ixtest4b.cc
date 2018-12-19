#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_4B(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Open Index File that created by 4a
    // 2. Scan entries NO_OP -- open**
    // 3. Scan close **
    // 4. Close Index File
    // 5. Destroy Index File
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 4b****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned key;
    int inRidPageNumSum = 0;
    int outRidPageNumSum = 0;
    unsigned numOfTuples = 1000;

    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // compute inRidPageNumSum without inserting entries
    for(unsigned i = 0; i <= numOfTuples; i++)
    {
        key = i;
        rid.pageNum = key;
        rid.slotNum = key+1;

        inRidPageNumSum += rid.pageNum;
    }

    // scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, NULL, NULL, true, true, ix_ScanIterator);

    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        if (rid.pageNum % 200 == 0) {
            cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        }
        outRidPageNumSum += rid.pageNum;
    }

    if (inRidPageNumSum != outRidPageNumSum)
    {
        cerr << "Wrong entries output...failure" << endl;
        ix_ScanIterator.close();
        return fail;
    }

    // close scan
    assertCloseIterator(success, ix_ScanIterator);

    // close index file
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    // Destroy Index
    assertDestroyIndexFile(success, indexManager, indexFileName);
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

    RC result = testCase_4B(indexFileName, attrAge);;
    if (result == success) {
        cerr << "IX_Test Case 4b passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 4b failed" << endl;
        return fail;
    }

}

