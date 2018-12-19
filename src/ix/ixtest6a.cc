#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_6A(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Create Index File
    // 2. Open Index File
    // 3. Insert entry with the same key
    // 4. Scan entries EXACT MATCH **
    // 5. Scan close **
    // 6. Close Index File
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 6A****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned key = 200;
    unsigned numOfTuples = 50;

    int inRidPageNumSum = 0;
    int outRidPageNumSum = 0;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    for(unsigned i = 0; i <= numOfTuples; i++)
    {
        rid.pageNum = i+1;
        rid.slotNum = i+2;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
        inRidPageNumSum += rid.pageNum;
    }

    // Scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            &key, &key, true, true, ix_ScanIterator);

    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        outRidPageNumSum += rid.pageNum;
    }

    if (inRidPageNumSum != outRidPageNumSum)
    {
        cerr << "Wrong entries output...failure" << endl;
        ix_ScanIterator.close();
        return fail;
    }

    // Close Scan
    assertCloseIterator(success, ix_ScanIterator);

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

    RC result = testCase_6A(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case 6a passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 6a failed" << endl;
        return fail;
    }

}

