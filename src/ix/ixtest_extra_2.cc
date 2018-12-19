#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_extra_2(const string &indexFileName, const Attribute &attribute)
{
    // Checks whether duplicated entries spaning multiple page are handled properly or not.
    //
    // Functions tested
    // 1. Create Index
    // 2. OpenIndex
    // 3. Insert entry
    // 4. Scan entries (EXACT MATCH).
    // 5. Scan close
    // 6. CloseIndex
    // 7. DestroyIndex
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case Extra 2****" << endl;

    RID rid;
    unsigned numOfTuples = 2000;
    unsigned numExtra = 500;
    unsigned key;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    int compVal1 = 1234, compVal2= 4321;
    int count = 0;

    //create index file(s)
    assertCreateIndexFile(success, indexManager, indexFileName);

    //open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    key = compVal1;
    for(unsigned i = 1; i <= numOfTuples; i++)
    {
        rid.pageNum = i;
        rid.slotNum = i;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }

    key = compVal2;
    for(unsigned i = numOfTuples; i < numOfTuples+ numExtra; i++)
    {
        rid.pageNum = i;
        rid.slotNum = i-5;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }

    //scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, 
            &compVal1, &compVal1, true, true, ix_ScanIterator);

    //iterate
    count = 0;
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        if(count % 1000 == 0)
            cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        count++;
    }

    cerr << "Number of scanned entries: " << count << endl;
    if (count != numOfTuples)
    {
        cerr << "Wrong entries output...failure" << endl;
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

    const string indexFileName = "age_idx";
    Attribute attrAge;
    attrAge.length = 4;
    attrAge.name = "age";
    attrAge.type = TypeInt;

    RC result = testCase_extra_2(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case Extra 2 passed." << endl;
        return success;
    } else {
        cerr << "IX_Test Case Extra 2 failed. Duplicated entries span multiple pages are not handled properly." << endl;
        return fail;
    }

}

