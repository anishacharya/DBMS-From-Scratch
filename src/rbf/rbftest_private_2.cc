#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <stdlib.h> 
#include <string.h>
#include <stdexcept>
#include <stdio.h> 

#include "pfm.h"
#include "rbfm.h"
#include "test_util.h"

using namespace std;

int RBFTest_private_2(RecordBasedFileManager *rbfm) {
	// Functions Tested:
	// 1. Create File - RBFM
	// 2. Open File
	// 3. insertTuple() - a big size tuple 
	// 4. Close File
	// 5. Destroy File
	cout << "***** In RBF Test Case Private 2 *****" << endl;

	RC rc;
	string fileName = "test_private_2";

   	unsigned readPageCount = 0;
	unsigned writePageCount = 0;
	unsigned appendPageCount = 0;
	unsigned readPageCount1 = 0;
	unsigned writePageCount1 = 0;
	unsigned appendPageCount1 = 0;
	unsigned readPageCount2 = 0;
	unsigned writePageCount2 = 0;
	unsigned appendPageCount2 = 0;

	// Create a file named "test_private_2"
	rc = rbfm->createFile(fileName);
    assert(rc == success && "Creating a file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating a file failed.");

    // Open the file "test_private_2"
    FileHandle fileHandle;
    rc = rbfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");
   
	RID rid;
	vector <RID> rids;
	vector <int> sizes;
	int recordSize = 0;
	void *record = malloc(3000);
	void *returnedData = malloc(3000);

	vector<Attribute> recordDescriptor;
	createLargeRecordDescriptor4(recordDescriptor);
    
    // NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptor.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);

    // Insert a record into the file
	prepareLargeRecord4(recordDescriptor.size(), nullsIndicator, 2060, record, &recordSize);
   
    // collect before counters
	rc = fileHandle.collectCounterValues(readPageCount, writePageCount, appendPageCount);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case Private 2 failed." << endl;
	    rc = rbfm->closeFile(fileHandle);
        return -1;
    }
     
    rc = rbfm->insertRecord(fileHandle, recordDescriptor, record, rid);
    assert(rc == success && "Inserting a record should not fail.");
    rids.push_back(rid);
    sizes.push_back(recordSize);

    // collect after counters - 1
	rc = fileHandle.collectCounterValues(readPageCount1, writePageCount1, appendPageCount1);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case Private 2 failed." << endl;
	    rc = rbfm->closeFile(fileHandle);
        return -1;
    }

	// Insert the second record into the file
	memset(record, 0, 3000);
	prepareLargeRecord4(recordDescriptor.size(), nullsIndicator, 2061, record, &recordSize);
    
    rc = rbfm->insertRecord(fileHandle, recordDescriptor, record, rid);
    assert(rc == success && "Inserting a record should not fail.");
    rids.push_back(rid);
    sizes.push_back(recordSize);

    // collect after counters - 2
	rc = fileHandle.collectCounterValues(readPageCount2, writePageCount2, appendPageCount2);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case Private 2 failed." << endl;
	    rc = rbfm->closeFile(fileHandle);
        return -1;
    }
    
    // Check (appendPageCount2 > appendPageCount1 > appendPageCount)
    if (appendPageCount2 <= appendPageCount1 || appendPageCount1 <= appendPageCount) {
    	cout << "The implementation regarding appendPage() is not correct." << endl;
    	cout << "***** [FAIL] Test Case Private 2 Failed! *****" << endl;
    	return -1;
    }
    
    // Given the rid, read the record from file
    rc = rbfm->readRecord(fileHandle, recordDescriptor, rid, returnedData);
    assert(rc == success && "Reading a record should not fail.");

    // Compare whether the two memory blocks are the same
    if(memcmp(record, returnedData, recordSize) != 0)
    {
        cout << "***** [FAIL] Test Case Private 2 Failed! *****" << endl << endl;
        free(record);
        free(returnedData);
        return -1;
    }
    
    // Close the file "test_private_2"
    rc = rbfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

	int fsize = getFileSize(fileName);
	if (fsize == 0) {
		cout << "File Size should not be zero at this moment." << endl;
        cout << "***** [FAIL] Test Case Private 2 Failed! *****" << endl << endl;
		return -1;
	}
	
    // Destroy File
    rc = rbfm->destroyFile(fileName);
    assert(rc == success && "Destroying the file should not fail.");
    
    rc = destroyFileShouldSucceed(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

    free(record);
    free(returnedData);
    
    cout << "***** [PASS] Test Case Private 2 Passed! *****" << endl << endl;
    
    return 0;
}

int main() {
	RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); // To test the functionality of the record-based file manager

	remove("test_private_2");

	RC rcmain = RBFTest_private_2(rbfm);
	return rcmain;
}
