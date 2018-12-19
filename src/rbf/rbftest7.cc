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

int RBFTest_7(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Create File
    // 2. Open File
    // 3. Append Page
    // 4. Get Number Of Pages
    // 5. Read Page
    // 6. Write Page
    // 7. Close File
    // 8. Destroy File
    cout << endl << "***** In RBF Test Case 7 *****" << endl;

    RC rc;
    string fileName = "test7";

	unsigned readPageCount = 0;
	unsigned writePageCount = 0;
	unsigned appendPageCount = 0;
	unsigned readPageCount1 = 0;
	unsigned writePageCount1 = 0;
	unsigned appendPageCount1 = 0;

    // Create the file named "test7"
    rc = pfm->createFile(fileName);
    assert(rc == success && "Creating the file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating the file failed.");

    // Open the file "test7"
    FileHandle fileHandle;
    rc = pfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");

    // collect before counters
	rc = fileHandle.collectCounterValues(readPageCount, writePageCount, appendPageCount);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case 7 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }

    // Append 50 pages
    void *data = malloc(PAGE_SIZE);
    for(unsigned j = 0; j < 50; j++)
    {
        for(unsigned i = 0; i < PAGE_SIZE; i++)
        {
            *((char *)data+i) = i % (j+1) + 32;
        }
        rc = fileHandle.appendPage(data);
        assert(rc == success && "Appending a page should not fail.");
    }
    cout << "50 Pages have been successfully appended!" << endl;

    // collect after counters
	rc = fileHandle.collectCounterValues(readPageCount1, writePageCount1, appendPageCount1);
    if(rc != success)
    {
        cout << "[FAIL] collectCounterValues() failed. Test Case 7 failed." << endl;
	    rc = pfm->closeFile(fileHandle);
        return -1;
    }
    cout << "before:R W A - " << readPageCount << " " << writePageCount << " " << appendPageCount << " after:R W A - " << readPageCount1 << writePageCount1 << " " << appendPageCount1 << endl;
    assert (appendPageCount1 > appendPageCount && "The appendPageCount should have been increased.");
   
    // Get the number of pages
    unsigned count = fileHandle.getNumberOfPages();
    assert(count == (unsigned)50 && "The count should be 50 at this moment.");

    // Read the 35th page and check integrity
    void *buffer = malloc(PAGE_SIZE);
    rc = fileHandle.readPage(34, buffer);
    assert(rc == success && "Reading a page should not fail.");

    for(unsigned i = 0; i < PAGE_SIZE; i++)
    {
        *((char *)data + i) = i % 35 + 32;
    }
    rc = memcmp(buffer, data, PAGE_SIZE);
    assert(rc == success && "Checking the integrity of a page should not fail.");
    cout << "The data in 35th page is correct!" << endl;

    // Update the 35th page
    for(unsigned i = 0; i < PAGE_SIZE; i++)
    {
        *((char *)data+i) = i % 60 + 32;
    }
    rc = fileHandle.writePage(34, data);
    assert(rc == success && "Writing a page should not fail.");

    // Read the 35th page and check integrity
    rc = fileHandle.readPage(34, buffer);
    assert(rc == success && "Reading a page should not fail.");
    
    rc = memcmp(buffer, data, PAGE_SIZE);
    assert(rc == success && "Checking the integrity of a page should not fail.");

    // Close the file "test7"
    rc = pfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

    // Destroy File
    rc = pfm->destroyFile(fileName);
    assert(rc == success && "Destroying the file should not fail.");

	rc = destroyFileShouldSucceed(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

    free(data);
    free(buffer);

	cout << "[PASS] Test Case 7 Passed!" << endl << endl;
    
    return 0;
    
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
     
    remove("test7");
       
    RC rcmain = RBFTest_7(pfm);
    return rcmain;
}
