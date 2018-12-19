#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_extra_1(const string &indexFileName, const Attribute &attribute)
{
    // Checks whether VARCHAR type is handled properly or not.
    //
    // Extra test case for Undergrad. Mandatory for Grad.
    // Pass: 5 extra points for Undergrad. No score deduction for Grad.
    // Fail: no extra points for Undergrad. Points will be deducted for Grad.
    //
    // Functions Tested:
    // 1. Create Index
    // 2. Open Index
    // 3. Insert Entry
    // 4. Get Insert IO count
    // 5. Scan
    // 6. Get Scan IO count
    // 7. Close Scan
    // 8. Close Index
    // 9. Destroy Index
    cerr << endl << "****In Extra Test Case 1****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned offset;
    unsigned numOfTuples = 5000;
    char key[100];
    unsigned count;

    //create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    //open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    int numOfTuplesTobeScanned = 0;

    // insert entry
    for(unsigned i = 1; i <= numOfTuples; i++)
    {
        count = ((i-1) % 26) + 1;
        *(int *)key = count;
        for(unsigned j = 0; j < count; j++)
        {
            *(key+4+j) = 96+count;
        }

        rid.pageNum = i;
        rid.slotNum = i;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, key, rid);

        if (count == 20) {
            numOfTuplesTobeScanned++;
        }
    }

    // collect counter
    // we will eyeball those numbers to see if they are reasonable.
    unsigned readPageCount, writePageCount, appendPageCount;
    assertCollectCounter(success, ixfileHandle, readPageCount, writePageCount, appendPageCount);
    cerr << " IO count after insertion: " 
        << " readPageCount: " << readPageCount
        << " writePageCount: " << writePageCount
        << " appendPageCount: " << appendPageCount << endl;

    //scan
    offset = 20;
    *(int *)key = offset;
    for(unsigned j = 0; j < offset; j++)
    {
        *(key+4+j) = 96+offset;
    }

    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, key, key, true, true, ix_ScanIterator);

    int count1 = 0;
    //iterate
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        cerr << "returned rid: " << rid.pageNum << " " << rid.slotNum << endl;
        if ( ((rid.pageNum - 1) % 26 + 1) != offset) {
            cerr << "Wrong entry output... Test failed..." << endl;
		cout<<"Desired Offset"<<offset<<endl;
            return fail;
        }
        count1++;
    }
    cerr << endl;

    if (count1 != numOfTuplesTobeScanned) {
	cout<<"TARGET"<<numOfTuplesTobeScanned<<endl;
        cerr << "Wrong entry output... Test failed..." << endl;
	
        ix_ScanIterator.close();
        indexManager->closeFile(ixfileHandle);
        indexManager->destroyFile(indexFileName);
        return fail;
    }

    // collect counter
    assertCollectCounter(success, ixfileHandle, readPageCount, writePageCount, appendPageCount);
    cerr << " IO count after insertion: " 
        << " readPageCount: " << readPageCount
        << " writePageCount: " << writePageCount
        << " appendPageCount: " << appendPageCount << endl;

    //close scan
    assertCloseIterator(success, ix_ScanIterator);

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
    attrEmpName.length = 100;
    attrEmpName.name = "EmpName";
    attrEmpName.type = TypeVarChar;

    RC result = testCase_extra_1(indexEmpNameFileName, attrEmpName);
    if (result == success) {
        cerr << "IX_Test Extra Case 1 passed." << endl;
        return success;
    } else {
        cerr << "IX_Test Extra Case 1 failed. VarChar type is not handled properly." << endl;
        return fail;
    }

}

