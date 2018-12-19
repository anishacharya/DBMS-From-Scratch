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

int RBFTest_1(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Create File
    cout << endl << "***** In RBF Test Case 1 *****" << endl;

    RC rc;
    string fileName = "test1";


    int recordSize = 0;
    void *record = malloc(100);
    void *returnedData = malloc(100);

    vector<Attribute> recordDescriptor;
    createRecordDescriptor(recordDescriptor);
    
    // Initialize a NULL field indicator
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(recordDescriptor.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullFieldsIndicatorActualSize);
	memset(nullsIndicator, 0, nullFieldsIndicatorActualSize);
	
    // Insert a record into a file
    prepareRecord(recordDescriptor.size(), nullsIndicator, 6, "Peters", 24, 170.1, 5000, record, &recordSize);

	// Null-indicators
    	bool nullBit = false;
	unsigned char *b = ((unsigned char *) record);
    	    //cout<<(int)b<<endl;
 	nullBit = b[0] & (1 << 7); 
 	cout << (int) b[0] << endl;
 	cout << (int) nullsIndicator[0] << endl;
 	cout << (bool) (b[0] & (1 << 4)) << endl;   	    
 	cout << (bool) (nullsIndicator[0] & (1 << 4)) << endl;   	    


    	    
    // Create a file named "test"
    rc = pfm->createFile(fileName);
    assert(rc == success && "Creating the file failed.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating the file failed.");

    // Create "test" again, should fail
    rc = pfm->createFile(fileName);
    assert(rc != success && "Creating the same file should fail.");

    cout << "[PASS] Test Case 1 Passed!" << endl << endl;
    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
    
    // Remove files that might be created by previous test run
    remove("test1");
    
    RC rcmain = RBFTest_1(pfm);
    return rcmain;
}
