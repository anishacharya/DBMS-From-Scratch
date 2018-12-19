#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_5b(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Create Index File
    // 2. Open Index File
    // 3. Insert entry
    // 4. Scan entries using LT_OP operator and checking if the values returned are correct.
    //    Returned values are part of two separate insertions. **
    // 5. Scan close
    // 6. Close Index File
    // 7. Destroy Index File
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 5b****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned numOfTuples = 2000;
    unsigned numOfMoreTuples = 6000;
    float key;
    float compVal = 6500;
    int inRidPageNumSum = 0;
    int outRidPageNumSum = 0;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    for(unsigned i = 1; i <= numOfTuples; i++)
    {
        key = (float)i + 76.5;
        rid.pageNum = i;
        rid.slotNum = i;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
        if (key < compVal)
        {
            inRidPageNumSum += rid.pageNum;
        }
    }

    for(unsigned i = 6000; i <= numOfTuples+numOfMoreTuples; i++)
    {
        key = (float)i + 76.5;
        rid.pageNum = i;
        rid.slotNum = i-(unsigned)500;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);

        if (key < compVal)
        {
            inRidPageNumSum += rid.pageNum;
        }
    }

    // scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            NULL, &compVal, true, false, ix_ScanIterator);

    // iterate
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        if(rid.pageNum % 500 == 0)
            cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
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

    // close index file(s)
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    //destroy index file(s)
    assertDestroyIndexFile(success, indexManager, indexFileName);

    return success;

}

int main()
{
    //Global Initializations
    indexManager = IndexManager::instance();

    const string indexFileName = "height_idx";
    Attribute attrHeight;
    attrHeight.length = 4;
    attrHeight.name = "height";
    attrHeight.type = TypeReal;

    RC result = testCase_5b(indexFileName, attrHeight);
    if (result == success) {
        cerr << "IX_Test Case 5b passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 5b failed" << endl;
        return fail;
    }

}

