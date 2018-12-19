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

int RBFTest_5(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Open File
    // 2. Read Page
    // 3. Close File
    cout << endl << "***** In RBF Test Case 5 *****" << endl;

    RC rc;
    string fileName = "test3";

	unsigned readPageCount = 0;
	unsigned writePageCount = 0;
	unsigned appendPageCount = 0;
	unsigned readPageCount1 = 0;
	unsigned writePageCount1 = 0;
	unsigned appendPageCount1 = 0;

    // Open the file "test3"
    FileHandle fileHandle;
    rc = pfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");

    // collect before counters
	rc = fileHandle.collectCounterValues(readPageCount, writePageCount, appendPageCount);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case 5 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }

    // Read the first page
    void *buffer = malloc(PAGE_SIZE);
    rc = fileHandle.readPage(0, buffer);
    assert(rc == success && "Reading a page should not fail.");
  
    // collect after counters
	rc = fileHandle.collectCounterValues(readPageCount1, writePageCount1, appendPageCount1);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case 5 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }
    cout << "before:R W A - " << readPageCount << " " << writePageCount << " " << appendPageCount << " after:R W A - " << readPageCount1 << writePageCount1 << " " << appendPageCount1 << endl;
    assert (readPageCount1 > readPageCount && "The readPageCount should have been increased.");

    // Check the integrity of the page    
    void *data = malloc(PAGE_SIZE);
    for(unsigned i = 0; i < PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 94 + 32;
    }
    rc = memcmp(data, buffer, PAGE_SIZE);
    assert(rc == success && "Checking the integrity of the page should not fail.");
 
    // Close the file "test3"
    rc = pfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

    free(data);
    free(buffer);

    cout << "[PASS] Test Case 5 Passed!" << endl << endl;

    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
        
    RC rcmain = RBFTest_5(pfm);
    return rcmain;
}
