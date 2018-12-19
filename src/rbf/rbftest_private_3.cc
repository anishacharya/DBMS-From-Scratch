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

int RBFTest_private_3(RecordBasedFileManager *rbfm) {
	// Functions Tested:
	// 1. Create a File - test_private_3a
	// 2. Create a File - test_private_3b
	// 3. Open test_private_3a
	// 4. Open test_private_3b
	// 5. Insert 50000 records into test_private_3a
	// 6. Insert 50000 records into test_private_3b
	// 7. Close test_private_3a
	// 8. Close test_private_3b
	cout << endl << "***** In RBF Test Case Private 3 ****" << endl;

	RC rc;
	string fileName = "test_private_3a";
	string fileName2 = "test_private_3b";

	// Create a file named "test_private_3a"
	rc = rbfm->createFile(fileName);
    assert(rc == success && "Creating a file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating a file should not fail.");

	// Create a file named "test_private_3b"
	rc = rbfm->createFile(fileName2);
    assert(rc == success && "Creating a file should not fail.");

	rc = createFileShouldSucceed(fileName2);
    assert(rc == success && "Creating a file should not fail.");

	// Open the file "test_private_3a"
	FileHandle fileHandle;
	rc = rbfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");

	// Open the file "test_private_3b"
	FileHandle fileHandle2;
	rc = rbfm->openFile(fileName2, fileHandle2);
    assert(rc == success && "Opening the file should not fail.");

	RID rid, rid2;
	void *record = malloc(2000);
	void *record2 = malloc(2000);
	void *returnedData = malloc(2000);
	void *returnedData2 = malloc(2000);
	int numRecords = 50000;

	vector<Attribute> recordDescriptorForTwitterUser, recordDescriptorForTweetMessage;

	createRecordDescriptorForTwitterUser(recordDescriptorForTwitterUser);
	createRecordDescriptorForTweetMessage(recordDescriptorForTweetMessage);

    // NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptorForTwitterUser.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);

    int nullFieldsIndicatorActualSize2 = getActualByteForNullsIndicator(recordDescriptorForTweetMessage.size());
    unsigned char *nullsIndicator2 = (unsigned char *) malloc(nullFieldsIndicatorActualSize2);
	memset(nullsIndicator2, 0, nullFieldsIndicatorActualSize2);

	vector<RID> rids, rids2;
	// Insert 50,000 records into the file - test_private_3a
	for (int i = 0; i < numRecords; i++) {
		// Test insert Record
		memset(record, 0, 2000);
		int size = 0;
		prepareLargeRecordForTwitterUser(recordDescriptorForTwitterUser.size(), nullsIndicator, i, record, &size);

		rc = rbfm->insertRecord(fileHandle, recordDescriptorForTwitterUser, record, rid);
	    assert(rc == success && "Inserting a record should not fail.");

		rids.push_back(rid);
		if (i%5000 == 0 && i != 0) {
			cout << "test_private_3a: " << i << " / " << numRecords << " records inserted so far." << endl;
		}
	}

	cout << "inserting " << numRecords << " records done." << endl << endl;
	
	// Insert 50,000 records into the file - test_private_3b
	for (int i = 0; i < numRecords; i++) {
		// Test insert Record
		memset(record2, 0, 2000);
		int size = 0;
		prepareLargeRecordForTweetMessage(recordDescriptorForTweetMessage.size(), nullsIndicator2, i, record2, &size);

		rc = rbfm->insertRecord(fileHandle2, recordDescriptorForTweetMessage, record2, rid2);
	    assert(rc == success && "Inserting a record should not fail.");

		rids2.push_back(rid2);
		if (i%5000 == 0 && i != 0) {
			cout << "test_private_3b: " << i << " / " << numRecords << " records inserted so far." << endl;
		}
	}

	cout << "inserting " << numRecords << " records done." << endl << endl;

	// Close the file - test_private_3a
	rc = rbfm->closeFile(fileHandle);
	assert(rc == success && "Closing a file should not fail.");

	free(record);
	free(returnedData);

	if (rids.size() != numRecords) {
        cout << "***** [FAIL] Test Case Private 3 Failed! *****" << endl << endl;
		return -1;
	}

	// Close the file - test_private_3b
	rc = rbfm->closeFile(fileHandle2);
	assert(rc == success && "Closing a file should not fail.");

	free(record2);
	free(returnedData2);

	if (rids2.size() != numRecords) {
        cout << "***** [FAIL] Test Case Private 3 Failed! *****" << endl << endl;
		return -1;
	}

	int fsize = getFileSize(fileName);
	if (fsize == 0) {
		cout << "File Size should not be zero at this moment." << endl;
        cout << "***** [FAIL] Test Case Private 3 Failed! *****" << endl << endl;
		return -1;
	}

	fsize = getFileSize(fileName2);
	if (fsize == 0) {
		cout << "File Size should not be zero at this moment." << endl;
        cout << "***** [FAIL] Test Case Private 3 Failed! *****" << endl << endl;
		return -1;
	}
		
	// Write RIDs of test_private_3a to a disk - do not use this code. This is not a page-based operation. For test purpose only.
	ofstream ridsFile("test_private_3a_rids", ios::out | ios::trunc | ios::binary);

	if (ridsFile.is_open()) {
		ridsFile.seekp(0, ios::beg);
		for (int i = 0; i < numRecords; i++) {
			ridsFile.write(reinterpret_cast<const char*>(&rids.at(i).pageNum),
					sizeof(unsigned));
			ridsFile.write(reinterpret_cast<const char*>(&rids.at(i).slotNum),
					sizeof(unsigned));
			if (i % 10000 == 0) {
				cout << "test_private_3a - RID #" << i << ": " << rids.at(i).pageNum
						<< ", " << rids.at(i).slotNum << endl;
			}
		}
		ridsFile.close();
		cout << endl << endl;
	}

	// Write RIDs of test_private_3b to a disk - do not use this code. This is not a page-based operation. For test purpose only.
	ofstream ridsFile2("test_private_3b_rids", ios::out | ios::trunc | ios::binary);

	if (ridsFile2.is_open()) {
		ridsFile2.seekp(0, ios::beg);
		for (int i = 0; i < numRecords; i++) {
			ridsFile2.write(reinterpret_cast<const char*>(&rids2.at(i).pageNum),
					sizeof(unsigned));
			ridsFile2.write(reinterpret_cast<const char*>(&rids2.at(i).slotNum),
					sizeof(unsigned));
			if (i % 10000 == 0) {
				cout << "test_private_3b - RID #" << i << ": " << rids2.at(i).pageNum
						<< ", " << rids2.at(i).slotNum << endl;
			}
		}
		ridsFile2.close();
	}

    cout << "***** Test Case Private 3 Finished. The result will be examined. *****" << endl << endl;

	return 0;
}

int main() {
	RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); // To test the functionality of the record-based file manager

	remove("test_private_3a");
	remove("test_private_3b");
	remove("test_private_3a_rids");
	remove("test_private_3b_rids");

	RC rcmain = RBFTest_private_3(rbfm);

	return rcmain;
}
