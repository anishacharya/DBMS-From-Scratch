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

int RBFTest_4(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Open File
    // 2. Append Page
    // 3. Get Number Of Pages
    // 4. Get Counter Values
    // 5. Close File
    cout << endl << "***** In RBF Test Case 4 *****" << endl;

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
        cout << "[FAIL] collectCounterValues() failed. Test Case 4 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }
	
    // Append the first page
    void *data = malloc(PAGE_SIZE);
    for(unsigned i = 0; i < PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 94 + 32;
    }
    rc = fileHandle.appendPage(data);
    assert(rc == success && "Appending a page should not fail.");
   
    // collect after counters
	rc = fileHandle.collectCounterValues(readPageCount1, writePageCount1, appendPageCount1);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case 4 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }
    cout << "before:R W A - " << readPageCount << " " << writePageCount << " " << appendPageCount << " after:R W A - " << readPageCount1 << writePageCount1 << " " << appendPageCount1 << endl;
    assert (appendPageCount1 > appendPageCount && "The appendPageCount should have been increased.");
   	
    // Get the number of pages
    unsigned count = fileHandle.getNumberOfPages();
    assert(count == (unsigned)1 && "The count should be one at this moment.");

    // Close the file "test3"
    rc = pfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

    free(data);

    cout << "[PASS] Test Case 4 Passed!" << endl << endl;

    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
        
    RC rcmain = RBFTest_4(pfm);
    return rcmain;
}
