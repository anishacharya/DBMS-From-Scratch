#include <iostream>
#include <cstdio>
#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int assertScanVailid(IXFileHandle &ixfileHandle, const Attribute &attribute, 
        IX_ScanIterator & ix_ScanIterator, const int inRecordNum){
    // Scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, NULL, NULL, true, true, ix_ScanIterator);

    int outRecordNum = 0;
    RID rid;
    unsigned key;
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {

        if (rid.pageNum != key +1 ||  rid.slotNum != key + 2){
            cerr << "Wrong entries output...failure" << endl;
            ix_ScanIterator.close();
            return fail;
        }
        outRecordNum += 1;
    }

    if (inRecordNum != outRecordNum)
    {
        cerr << "Wrong entries output...failure" << endl;
        ix_ScanIterator.close();
        return fail;
    }
    return success;
}

int testCase_LargeDataSet(const string &indexFileName, const Attribute &attribute){
    // Create Index file
    // Open Index file
    // Insert large number of records
    // Scan large number of records to validate insert correctly
    // Delete  some
    // Insert large number of records again
    // Scan large number of records to validate insert correctly
    // Delete all
    // Close Index
    // Destry Index

    cerr << endl << "****In Test Case 7 - LargeDataSet****" << endl;

    RID rid;
    IXFileHandle ixfileHandle;
    IX_ScanIterator ix_ScanIterator;
    unsigned key;
    int inRecordNum = 0;
    unsigned numOfTuples = 1000 * 1000;

    // create index file
    assertCreateIndexFile(success, indexManager, indexFileName);

    // open index file
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // insert entry
    for(unsigned i = 0; i <= numOfTuples; i++)
    {
        key = i; 
        rid.pageNum = key+1;
        rid.slotNum = key+2;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
        inRecordNum += 1;
    }

    // scan
    int rc = assertScanVailid(ixfileHandle, attribute, ix_ScanIterator, inRecordNum);
    if (rc != success){
        cerr << "Scan inserted results failed" << endl;
        return fail;
    }

    // Delete some 
    int deletedRecord = 0;
    for(unsigned i = 0; i <= numOfTuples; i+=10)
    {
        key = i; 
        rid.pageNum = key+1;
        rid.slotNum = key+2;

        assertDeleteEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
        deletedRecord += 1;
    }

    // scan 
    rc = assertScanVailid(ixfileHandle, attribute, ix_ScanIterator, 
            inRecordNum - deletedRecord);
    if (rc != success){
        cerr << "Scan inserted results after deletion failed" << endl;
        return fail;
    }

    // Insert again
    for(unsigned i = 0; i <= numOfTuples; i+=10)
    {
        key = i; 
        rid.pageNum = key+1;
        rid.slotNum = key+2;

        assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);
    }

    // Scan to verify the result
    rc = assertScanVailid(ixfileHandle, attribute, ix_ScanIterator, inRecordNum);
    if (rc != success){
        cerr << "Scan inserted results failed" << endl;
        return fail;
    }

    // Close Scan
    assertCloseIterator(success, ix_ScanIterator);

    // Close Index
    assertCloseIndexFile(success, indexManager, ixfileHandle);

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

    RC result = testCase_LargeDataSet(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case 7 - Large DataSet passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 7 - Large DataSet failed" << endl;
        return fail;
    }

}

