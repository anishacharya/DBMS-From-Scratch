#include <fstream>
#include <iostream>
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

int RBFTest_10(RecordBasedFileManager *rbfm) {
    // Functions tested
    // 1. Open Record-Based File
    // 2. Read Multiple Records
    // 3. Close Record-Based File
    // 4. Destroy Record-Based File
    cout << endl << "***** In RBF Test Case 10 *****" << endl;
   
    RC rc;
    string fileName = "test9";

    // Open the file "test9"
    FileHandle fileHandle;
    rc = rbfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");
    
    int numRecords = 2000;
    void *record = malloc(1000);
    void *returnedData = malloc(1000);

    vector<Attribute> recordDescriptor;
    createLargeRecordDescriptor(recordDescriptor);

    vector<RID> rids;
    vector<int> sizes;
    RID tempRID;

	// Read rids from the disk - do not use this code. This is not a page-based operation. For test purpose only.
	ifstream ridsFileRead("test9rids", ios::in | ios::binary);

	unsigned pageNum;
	unsigned slotNum;

	if (ridsFileRead.is_open()) {
		ridsFileRead.seekg(0,ios::beg);
		for (int i = 0; i < numRecords; i++) {
			ridsFileRead.read(reinterpret_cast<char*>(&pageNum), sizeof(unsigned));
			ridsFileRead.read(reinterpret_cast<char*>(&slotNum), sizeof(unsigned));
			if (i % 1000 == 0) {
				cout << "loaded RID #" << i << ": " << pageNum << ", " << slotNum << endl;
			}
			tempRID.pageNum = pageNum;
			tempRID.slotNum = slotNum;
			rids.push_back(tempRID);
		}
		ridsFileRead.close();
	}

	assert(rids.size() == (unsigned) numRecords && "Reading records should not fail.");


	// Read sizes vector from the disk - do not use this code. This is not a page-based operation. For test purpose only.
	ifstream sizesFileRead("test9sizes", ios::in | ios::binary);

	int tempSize;
	
	if (sizesFileRead.is_open()) {
		sizesFileRead.seekg(0,ios::beg);
		for (int i = 0; i < numRecords; i++) {
			sizesFileRead.read(reinterpret_cast<char*>(&tempSize), sizeof(int));
			if (i % 1000 == 0) {
				cout << "loaded Sizes #" << i << ": " << tempSize << endl;
			}
			sizes.push_back(tempSize);
		}
		sizesFileRead.close();
	}

	assert(sizes.size() == (unsigned) numRecords && "Reading records should not fail.");

    // NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptor.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);

    for(int i = 0; i < numRecords; i++)
    {
        memset(record, 0, 1000);
        memset(returnedData, 0, 1000);
        rc = rbfm->readRecord(fileHandle, recordDescriptor, rids[i], returnedData);
        assert(rc == success && "Opening the file should not fail.");
        
        cout << "Returned Data:" << endl;
        rbfm->printRecord(recordDescriptor, returnedData);
	cout<< "Failed Here test 10 -1"<<endl;
        int size = 0;
        prepareLargeRecord(recordDescriptor.size(), nullsIndicator, i, record, &size);
        if(memcmp(returnedData, record, sizes[i]) != 0)
        {
            cout << "[FAIL] Test Case 10 Failed!" << endl << endl;
            free(record);
            free(returnedData);
            return -1;
        }
    }
    
    // Close the file "test9"
    rc = rbfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");
    
    rc = rbfm->destroyFile(fileName);
    assert(rc == success && "Destroying the file should not fail.");

	rc = destroyFileShouldSucceed(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

	free(record);
	free(returnedData);

	cout << "[PASS] Test Case 10 Passed!" << endl << endl;

	remove("test9sizes");
	remove("test9rids");
	
	return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    // PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
     
    RC rcmain = RBFTest_10(rbfm);
    return rcmain;
}
