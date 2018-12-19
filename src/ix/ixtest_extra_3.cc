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

int testCase_VerifyTopDownDeletion(const string &indexFileName, 
        const Attribute &attribute){
    // Checks whether the deletion is in the TopDown manner
    // Functions tested
    // 1. CreateIndex
    // 2. OpenIndex
    // 3. Insert entries to make a 3 level tree 
    // 4. Print BTree 
    // 5. Delete the "unsafe one" 
    // 6. CloseIndex
    // 7. DestroyIndex
    cerr << endl << "****In Test Case Extra 3 - VeriyTopDownInsertion ****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned numOfTuples = 9;
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

    // print BTree, by this time the BTree should have 3 level
    indexManager->printBtree(ixfileHandle, attribute);

    // delete the 2nd
    fillUpKeyRid(count, 2, key, rid);
    assertDeleteEntry(success, indexManager, ixfileHandle, attribute, key, rid);
    // print BTree, by this time the BTree should have 2 level
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

    RC result = testCase_VerifyTopDownDeletion(indexEmpNameFileName, attrEmpName);
    if (result == success) {
        cerr << "IX_Test Case Extra 3 - topdown deletion finished, please eyeball the shape of BTee to varify the correctness." << endl;
        return success;
    } else {
        cerr << "IX_Test Case Extra 3 - topdown deletion failed" << endl;
        return fail;
    }

}



