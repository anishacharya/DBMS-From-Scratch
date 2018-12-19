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

int RBFTest_extra_1(RecordBasedFileManager *rbfm) {
	// Checks whether VarChar is implemented correctly or not.
	// Mandatory for the Grad, extra credit for the Undergrad
	//
	// Functions tested
	// 1. Create Two Record-Based File
	// 2. Open Two Record-Based File
	// 3. Insert Multiple Records Into Two files
	// 4. Close Two Record-Based File
	// 5. Compare The File Sizes
	// 6. Destroy Two Record-Based File
	cout << "****In RBF Test Case Extra 1****" << endl;

	RC rc;
	string fileName1 = "test_extra_1a";
	string fileName2 = "test_extra_1b";

	// Create a file named "test_extra_1a"
	rc = rbfm->createFile(fileName1);
    assert(rc == success && "Creating a file should not fail.");

	rc = createFileShouldSucceed(fileName1);
    assert(rc == success && "Creating a file failed.");

	// Create a file named "test_extra_1b"
	rc = rbfm->createFile(fileName2);
    assert(rc == success && "Creating a file should not fail.");

	rc = createFileShouldSucceed(fileName2);
    assert(rc == success && "Creating a file failed.");

	// Open the file "test_extra_1a"
	FileHandle fileHandle1;
	rc = rbfm->openFile(fileName1, fileHandle1);
    assert(rc == success && "Opening a file should not fail.");

	// Open the file "test_extra_1b"
	FileHandle fileHandle2;
	rc = rbfm->openFile(fileName2, fileHandle2);
    assert(rc == success && "Opening a file should not fail.");

	RID rid;
	void *record = malloc(3000);
	int numRecords = 10000;

	vector<Attribute> recordDescriptor1;
	createRecordDescriptorForTwitterUser(recordDescriptor1);

    // NULL field indicator
    int nullFieldsIndicatorActualSize1 = getActualByteForNullsIndicator(recordDescriptor1.size());
    unsigned char *nullsIndicator1 = (unsigned char *) malloc(nullFieldsIndicatorActualSize1);
	memset(nullsIndicator1, 0, nullFieldsIndicatorActualSize1);

	vector<Attribute> recordDescriptor2;
	createRecordDescriptorForTwitterUser2(recordDescriptor2);

	// Insert 10000 records into file
	for (int i = 0; i < numRecords; i++) {
		// Test insert Record
		int size = 0;
		memset(record, 0, 3000);
		prepareLargeRecordForTwitterUser(recordDescriptor1.size(), nullsIndicator1, i, record, &size);

		rc = rbfm->insertRecord(fileHandle1, recordDescriptor1, record, rid);
	    assert(rc == success && "Inserting a record should not fail.");

		rc = rbfm->insertRecord(fileHandle2, recordDescriptor2, record, rid);
	    assert(rc == success && "Inserting a record should not fail.");

	    if (i%2000 == 0 && i != 0) {
	    	cout << i << " / " << numRecords << " records are inserted." << endl;
	    }
	}
	// Close the file "test_extra_1a"
	rc = rbfm->closeFile(fileHandle1);
    assert(rc == success && "Closing a file should not fail.");

	// Close the file "test_extra_1b"
	rc = rbfm->closeFile(fileHandle2);
    assert(rc == success && "Closing a file should not fail.");

    free(record);

	bool equalSizes = compareFileSizes(fileName1, fileName2);

	rc = rbfm->destroyFile(fileName1);
    assert(rc == success && "Destroying a file should not fail.");

	rc = destroyFileShouldSucceed(fileName1);
    assert(rc == success  && "Destroying the file should not fail.");

	rc = rbfm->destroyFile(fileName2);
    assert(rc == success && "Destroying a file should not fail.");

	rc = destroyFileShouldSucceed(fileName2);
    assert(rc == success  && "Destroying the file should not fail.");

	if (!equalSizes) {
		cout << "Variable length Record is not properly implemented." << endl;
		cout << "**** [FAIL] RBF Test Extra 1 failed. Two files are of different sizes. *****" << endl;
		return -1;
	}
	
	cout << "**** RBF Test Extra 1 Finished. The result will be examined. *****" << endl;
	
	return 0;
}

int main() {
	RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); // To test the functionality of the record-based file manager

	remove("test_extra_1a");
	remove("test_extra_1b");

	RC rcmain = RBFTest_extra_1(rbfm);
	return rcmain;
}
