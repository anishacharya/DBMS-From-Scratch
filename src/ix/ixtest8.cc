#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

void fillUpKeyRid(const unsigned count, const unsigned i, char* key, RID &rid){
    *(int *)key = count;
    for(unsigned j = 0; j < count; j++)
    {
        *(key+4+j) = 96+i;
    }
    rid.pageNum = i;
    rid.slotNum = i;
}

int testCase_VerifyTopDownInsert(const string &indexFileName, 
        const Attribute &attribute){
    // Checks whether the insertion is in the TopDown manner
    // Functions tested
    // 1. CreateIndex
    // 2. OpenIndex
    // 3. Insert entries to make root full
    // 4. Print BTree 
    // 5. Insert one more entries to watch the shape of the BTree
    // 6. CloseIndex
    // 7. DestroyIndex
    cerr << endl << "****In Test 8 - VeriyTopDownInsertion ****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned numOfTuples = 7;
    char key[PAGE_SIZE];
    unsigned count = attribute.length;
    //create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    //open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    unsigned i = 1;
    for(; i <= numOfTuples; i++)
    {
        fillUpKeyRid(count, i, key, rid);
        assertInsertEntry(success, indexManager, ixfileHandle, attribute, key, rid);
    }

    // print BTree, by this time the BTree should have 2 level
    indexManager->printBtree(ixfileHandle, attribute);

    // depend on the implementation of root, it could be split at the 8th or 9th insertion

    // insert the 8th
    fillUpKeyRid(count, i++, key, rid);
    assertInsertEntry(success, indexManager, ixfileHandle, attribute, key, rid);
    // print BTree, by this time the BTree should have 2 or 3 level 
    // depend on the design of your root
    indexManager->printBtree(ixfileHandle, attribute);

    // insert the 9th
    fillUpKeyRid(count, i++, key, rid);
    assertInsertEntry(success, indexManager, ixfileHandle, attribute, key, rid);
    // print BTree, by this time the BTree should have 3 level
    indexManager->printBtree(ixfileHandle, attribute);

    //close index file
    assertCloseIndexFile(success, indexManager, ixfileHandle);

    //destroy index file
    assertDestroyIndexFile(success, indexManager, indexFileName);
    return success;

}

int main()
{
    //Global Initializations
    indexManager = IndexManager::instance();

    const string indexEmpNameFileName = "EmpName_idx";

    Attribute attrEmpName;
    attrEmpName.length = PAGE_SIZE / 4;  // each node could only have 3 children
    attrEmpName.name = "EmpName";
    attrEmpName.type = TypeVarChar;

    RC result = testCase_VerifyTopDownInsert(indexEmpNameFileName, attrEmpName);
    if (result == success) {
        cerr << "IX_Test 8 - topdown insertion finished, please eyeball the shape of BTee to varify the correctness." << endl;
        return success;
    } else {
        cerr << "IX_Test 8 - topdown insertion failed" << endl;
        return fail;
    }

}



