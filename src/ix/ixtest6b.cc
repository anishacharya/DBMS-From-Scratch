#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_6B(const string &indexFileName, const Attribute &attribute)
{
    // Checks whether duplicated entries are handled by RIDList or not
    //
    // Functions tested
    // 1. OpenIndex
    // 2. Insert entries with the same key
    // 3. Print BTree **
    // 4. CloseIndex
    // 5. DestroyIndex
    // NOTE: "**" signifies the new functions being tested in this test case.

    cerr << endl << "****In Test Case 6B****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned key = 300;
    unsigned numOfTuples = 50;

    int inRidPageNumSum = 0;

    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    for(unsigned i = 0; i <= numOfTuples; i++)
    {
        rid.pageNum = numOfTuples + i+1;
        rid.slotNum = i+2;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
        inRidPageNumSum += rid.pageNum;
    }

    // eyeball check a key only appears once in each node (both innernodes and leaves)
    indexManager->printBtree(ixfileHandle, attribute);

    // Close Index
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    // destroy index file
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

    RC result = testCase_6B(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case 6B works, " 
            << "please eyeball check if the RIDList is implemented correctly" 
            << endl;
        return success;
    } else {
        cerr << "IX_Test Case 6B failed. entries are not handled properly." << endl;
        return fail;
    }

}

