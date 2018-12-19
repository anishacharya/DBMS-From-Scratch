#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_9(const string &indexFileName, const Attribute &attribute)
{
    // Checks whether deleting an entry after getNextEntry() for EXACT MATCH case is handled properly or not.
    //

    // Functions tested
    // 1. Create Index File
    // 2. OpenIndex File
    // 3. Insert entry
    // 4. Scan entries - EXACT MATCH, and delete entries
    // 5. Scan close
    // 6. CloseIndex File
    // 7. DestroyIndex File
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test 9****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    float compVal = 100.0;
    unsigned numOfTuples = 500;
    float key;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);
    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // InsertEntry
    key = compVal;

    for(unsigned i = 1; i <= numOfTuples; i++)
    {
        rid.pageNum = i;
        rid.slotNum = i;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }

    // scan - EXACT MATCH
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            &compVal, &compVal, true, true, ix_ScanIterator);

    // DeleteEntry in IndexScan Iterator
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        if (rid.pageNum % 100 == 0) {
            cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        }
        assertDeleteEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }
    cerr << endl;

    // close scan
    assertCloseIterator(success, ix_ScanIterator);

    // Open Scan again
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            &compVal, &compVal, true, true, ix_ScanIterator);

    bool testFailed = false;

    //iterate
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        cerr << "Entry returned: " << rid.pageNum << " " << rid.slotNum << " --- Test failed." << endl;
        testFailed = true;
        break;
    }

    if (testFailed) {
        cerr << "Wrong entries output... failure" << endl;
        ix_ScanIterator.close();
        return fail;
    }

    //close scan
    assertCloseIterator(success, ix_ScanIterator);
    //close index file file
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    //destroy index file file
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

    RC result = testCase_9(indexFileName, attrHeight);
    if (result == success) {
        cerr << "IX_Test 9 passed." << endl;
        return success;
    } else {
        cerr << "IX_Test 9 failed.  " << endl;
        return fail;
    }

}

