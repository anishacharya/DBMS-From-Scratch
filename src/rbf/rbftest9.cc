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

int RBFTest_9(RecordBasedFileManager *rbfm, vector<RID> &rids, vector<int> &sizes) {
    // Functions tested
    // 1. Create Record-Based File
    // 2. Open Record-Based File
    // 3. Insert Multiple Records
    // 4. Close Record-Based File
    cout << endl << "***** In RBF Test Case 9 *****" << endl;
   
    RC rc;
    string fileName = "test9";

    // Create a file named "test9"
    rc = rbfm->createFile(fileName);
    assert(rc == success && "Creating the file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating the file failed.");

    // Open the file "test9"
    FileHandle fileHandle;
    rc = rbfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");

    RID rid; 
    void *record = malloc(1000);
    int numRecords = 2000;

    vector<Attribute> recordDescriptor;
    createLargeRecordDescriptor(recordDescriptor);

    for(unsigned i = 0; i < recordDescriptor.size(); i++)
    {
        cout << "Attr Name: " << recordDescriptor[i].name << " Attr Type: " << (AttrType)recordDescriptor[i].type << " Attr Len: " << recordDescriptor[i].length << endl;
    }
	cout << endl;
	
    // NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptor.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);

    // Insert 2000 records into file
    for(int i = 0; i < numRecords; i++)
    {
        // Test insert Record
        int size = 0;
        memset(record, 0, 1000);
        prepareLargeRecord(recordDescriptor.size(), nullsIndicator, i, record, &size);

        rc = rbfm->insertRecord(fileHandle, recordDescriptor, record, rid);
        assert(rc == success && "Inserting the file should not fail.");

        rids.push_back(rid);
        sizes.push_back(size);        
    }
    // Close the file "test9"
    rc = rbfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

    free(record);
    
    
    // Write RIDs to the disk. Do not use this code. This is not a page-based operation - for the test purpose only.
	ofstream ridsFile("test9rids", ios::out | ios::trunc | ios::binary);

	if (ridsFile.is_open()) {
		ridsFile.seekp(0, ios::beg);
		for (int i = 0; i < numRecords; i++) {
			ridsFile.write(reinterpret_cast<const char*>(&rids[i].pageNum),
					sizeof(unsigned));
			ridsFile.write(reinterpret_cast<const char*>(&rids[i].slotNum),
					sizeof(unsigned));
			if (i % 1000 == 0) {
				cout << "RID #" << i << ": " << rids[i].pageNum << ", "
						<< rids[i].slotNum << endl;
			}
		}
		ridsFile.close();
	}

    // Write sizes vector to the disk. Do not use this code. This is not a page-based operation - for the test purpose only.
	ofstream sizesFile("test9sizes", ios::out | ios::trunc | ios::binary);

	if (sizesFile.is_open()) {
		sizesFile.seekp(0, ios::beg);
		for (int i = 0; i < numRecords; i++) {
			sizesFile.write(reinterpret_cast<const char*>(&sizes[i]),sizeof(int));
			if (i % 1000 == 0) {
				cout << "Sizes #" << i << ": " << sizes[i] << endl;
			}
		}
		sizesFile.close();
	}
        
    cout << "[PASS] Test Case 9 Passed!" << endl << endl;

    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    // PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 

    remove("test9");
    remove("test9rids");
    remove("test9sizes");

    vector<RID> rids;
    vector<int> sizes;    
    RC rcmain = RBFTest_9(rbfm, rids, sizes);
    return rcmain;
}
