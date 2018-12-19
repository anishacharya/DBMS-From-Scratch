#include <iostream>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "ix.h"
#include "ixtest_util.h"

IndexManager *indexManager;

int testCase_2(const string &indexFileName, const Attribute &attribute)
{
    // Functions tested
    // 1. Open Index file
    // 2. Insert entry **
    // 3. Disk I/O check of Insertion - CollectCounterValues **
    // 4. Disk I/O check of Scan and getNextEntry - CollectCounterValues **
    // 5. Delete entry **
    // 6. Disk I/O check of Deletion - CollectCounterValues **
    // 7. Delete entry -- when the value is not there: should fail **
    // 8. Close Index file
    // NOTE: "**" signifies the new functions being tested in this test case.
    cerr << endl << "****In Test Case 2****" << endl;

    RID rid;
    int key = 100;
    rid.pageNum = key;
    rid.slotNum = key+1;

    unsigned readPageCount = 0;
    unsigned writePageCount = 0;
    unsigned appendPageCount = 0;
    unsigned readPageCountAfter = 0;
    unsigned writePageCountAfter = 0;
    unsigned appendPageCountAfter = 0;
    unsigned readDiff = 0;
    unsigned writeDiff = 0;
    unsigned appendDiff = 0;

    IX_ScanIterator ix_ScanIterator;

    // open index file
    IXFileHandle ixfileHandle;
    assertOpenIndexFile(success, indexManager, indexFileName, ixfileHandle);

    // collect counters
    assertCollectCounter(success, ixfileHandle, readPageCount, writePageCount, appendPageCount);

    cerr << endl << "Before Insert - readPageCount:" << readPageCount 
        << " writePageCount:" << writePageCount 
        << " appendPageCount:" << appendPageCount 
        << endl;

    // insert entry
    assertInsertEntry(success, indexManager, ixfileHandle, attribute, &key, rid);

    // collect counters
    assertCollectCounter(success, ixfileHandle, 
            readPageCountAfter, writePageCountAfter, appendPageCountAfter);

    cerr << "After Insert - readPageCount:" << readPageCountAfter 
        << " writePageCount:" << writePageCountAfter 
        << " appendPageCount:" << appendPageCountAfter 
        << endl;

    readDiff = readPageCountAfter - readPageCount;
    writeDiff = writePageCountAfter - writePageCount;
    appendDiff = appendPageCountAfter - appendPageCount;

    cerr << "Page I/O count of single insertion - readPage:" << readDiff 
        << " writePageCount:" << writeDiff 
        << " appendPage:" << appendDiff 
        << endl;

    if (readDiff == 0 && writeDiff == 0 && appendDiff == 0) {
        cerr << "Insertion should generate some page I/O. The implementation is not correct." << endl;
        return fail;
    } 

    // collect counters
    assertCollectCounter(success, ixfileHandle, readPageCount, writePageCount, appendPageCount);

    cerr << endl << "Before scan - readPageCount:" << readPageCount 
        << " writePageCount:" << writePageCount 
        << " appendPageCount:" << appendPageCount 
        << endl;

    // Conduct a scan
    assertInitalizeScan(success, indexManager, ixfileHandle, attribute, NULL, NULL, true, true, ix_ScanIterator);

    // should be one record
    int count = 0;
    while(ix_ScanIterator.getNextEntry(rid, &key) == success)
    {
        cerr << "Returned rid from a scan: " << rid.pageNum << " " << rid.slotNum << endl;
        assert(rid.pageNum == key);
        assert(rid.slotNum == key+1);
        count ++;
    }
    assert(count == 1);

    // collect counters
    assertCollectCounter(success, ixfileHandle, readPageCountAfter, writePageCountAfter, appendPageCountAfter);

    cerr << "After scan - readPageCount:" << readPageCountAfter 
        << " writePageCount:" << writePageCountAfter 
        << " appendPageCount:" << appendPageCountAfter 
        << endl;

    readDiff = readPageCountAfter - readPageCount;
    writeDiff = writePageCountAfter - writePageCount;
    appendDiff = appendPageCountAfter - appendPageCount;

    cerr << "Page I/O count of scan - readPage:" << readDiff 
        << " writePageCount:" << writeDiff 
        << " appendPage:" << appendDiff 
        << endl;

    if (readDiff == 0 && writeDiff == 0 && appendDiff == 0) {
        cerr << "Scan should generate some page I/O. The implementation is not correct." << endl;
        return fail;
    } 

    // collect counters
    assertCollectCounter(success, ixfileHandle, readPageCount, writePageCount, appendPageCount);

    cerr << endl << "Before Delete - readPageCount:" << readPageCount 
        << " writePageCount:" << writePageCount 
        << " appendPageCount:" << appendPageCount << endl;

    // delete entry
    assertDeleteEntry(success, indexManager, ixfileHandle, attribute, &key, rid);

    // collect counters
    assertCollectCounter(success, ixfileHandle, readPageCountAfter, writePageCountAfter, appendPageCountAfter);

    cerr << "After Delete - readPageCount:" << readPageCountAfter 
        << " writePageCount:" << writePageCountAfter 
        << " appendPageCount:" << appendPageCountAfter << endl;

    readDiff = readPageCountAfter - readPageCount;
    writeDiff = writePageCountAfter - writePageCount;
    appendDiff = appendPageCountAfter - appendPageCount;

    cerr << "Page I/O count of single delete - readPage:" << readDiff 
        << " writePageCount:" << writeDiff 
        << " appendPage:" << appendDiff << endl << endl;

    if (readDiff == 0 && writeDiff == 0 && appendDiff == 0) {
        cerr << "Deletion should generate some page I/O. The implementation is not correct." << endl;
        indexManager->closeFile(ixfileHandle);		
        return fail;
    } 

    // delete entry again
    // This time it should NOT give success because entry is not there.
    assertDeleteEntry(fail, indexManager, ixfileHandle, attribute, &key, rid);

    // close index file
    assertCloseIndexFile(success, indexManager, ixfileHandle);
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

    RC result = testCase_2(indexFileName, attrAge);
    if (result == success) {
        cerr << "IX_Test Case 2 passed" << endl;
        return success;
    } else {
        cerr << "IX_Test Case 2 failed" << endl;
        return fail;
    }

}

