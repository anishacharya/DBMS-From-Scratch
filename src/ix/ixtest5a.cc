#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_5a(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Create Index File
    // 2. Open Index File
    // 3. Insert entry
    // 4. Scan entries using GE_OP operator and checking if the values returned are correct. **
    // 5. Scan close
    // 6. Close Index File
    // 7. Destroy Index File
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 5a****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned numOfTuples = 300;
    unsigned numOfMoreTuples = 100;
    unsigned key;
    int inRidPageNumSum = 0;
    int outRidPageNumSum = 0;
    int value = 5001;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);
    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // Test Insert Entry
    for(unsigned i = 1; i <= numOfTuples; i++)
    {
        key = i;
        rid.pageNum = key;
        rid.slotNum = key+1;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }

    for(unsigned i = value; i < value + numOfMoreTuples; i++)
    {
        key = i;
        rid.pageNum = key;
        rid.slotNum = key+1;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);

        inRidPageNumSum += rid.pageNum;
    }

    // Test Open Scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            &value, NULL, true, true, ix_ScanIterator);

    // Test IndexScan iterator
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        if (rid.pageNum % 100 == 0) {
            cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        }
        if (rid.pageNum < value || rid.slotNum < value + 1)
        {
            cerr << "Wrong entries output...failure" << endl;
            ix_ScanIterator.close();
            return fail;
        }
        outRidPageNumSum += rid.pageNum;
    }

    if (inRidPageNumSum != outRidPageNumSum)
    {
        cerr << "Wrong entries output...failure" << endl;
        ix_ScanIterator.close();
        return fail;
    }

    // Test Closing Scan
    assertCloseIterator(success, ix_ScanIterator);

    // Test Closing Index
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    // Test Destroying Index
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

    RC result = testCase_5a(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case 5a passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 5a failed" << endl;
        return fail;
    }

}

