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

int RBFTest_private_1b(RecordBasedFileManager *rbfm) {
	// Functions Tested:
	// 1. Create File - RBFM
	// 2. Open File
	// 3. insertTuple() - with three NULLs
	// 4. Close File
	// 5. Destroy File
	cout << "***** In RBF Test Case Private 1b *****" << endl;

	RC rc;
	string fileName = "test_private_1b";

    // Create a file named "test_private_1b"
    rc = rbfm->createFile(fileName);
    assert(rc == success && "Creating the file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating the file should not fail.");

    // Open the file "test_private_1b"
    FileHandle fileHandle;
    rc = rbfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");
   
	RID rid;
	int recordSize = 0;
	void *record = malloc(2000);
	void *returnedData = malloc(2000);

	vector<Attribute> recordDescriptor;
	createLargeRecordDescriptor3(recordDescriptor);
    
    // NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptor.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);

	// Setting attr0, attr8, and attr16 as NULL. 
	// Each of theses attributes is placed as the left-most bit in each byte.
	// The entire byte representation is: 100000010000001000000
	nullsIndicator[0] = 128; // 10000000
	nullsIndicator[1] = 128; // 10000000
	nullsIndicator[2] = 128; // 10000000
	
    // Insert a record into a file
	prepareLargeRecord3(recordDescriptor.size(), nullsIndicator, 5, record, &recordSize);
    
	// The attr0, attr8, and attr16 fields should not be printed
    cout << "Insert Data:" << endl;
    rbfm->printRecord(recordDescriptor, record);
    
    rc = rbfm->insertRecord(fileHandle, recordDescriptor, record, rid);
    assert(rc == success && "Inserting a record should not fail.");
    
    // Given the rid, read the record from file
    rc = rbfm->readRecord(fileHandle, recordDescriptor, rid, returnedData);
    assert(rc == success && "Reading a record should not fail.");

	// The attr0, attr8, and attr16 fields should not be printed
    cout << "Returned Data:" << endl;
    rbfm->printRecord(recordDescriptor, returnedData);

    // Compare whether the two memory blocks are the same
    if(memcmp(record, returnedData, recordSize) != 0)
    {
        cout << "***** [FAIL] Test Case Private 1b Failed! *****" << endl << endl;
        free(record);
        free(returnedData);
        return -1;
    }
    
    // Close the file "test_private_1b"
    rc = rbfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

	int fsize = getFileSize(fileName);
	if (fsize == 0) {
		cout << "File Size should not be zero at this moment." << endl;
		return -1;
	}
	
    // Destroy File
    rc = rbfm->destroyFile(fileName);
    assert(rc == success && "Destroying the file should not fail.");
    
    rc = destroyFileShouldSucceed(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

    free(record);
    free(returnedData);
    
    cout << "***** Test Case Private 1b Finished! The result will be examined. *****" << endl << endl;
    
    return 0;
}

int main() {
	RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); // To test the functionality of the record-based file manager

	remove("test_private_1b");

	RC rcmain = RBFTest_private_1b(rbfm);
	return rcmain;
}
