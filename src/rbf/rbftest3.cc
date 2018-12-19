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

int RBFTest_3(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Create File
    // 2. Open File
    // 3. Get Number Of Pages
    // 4. Close File
    cout << endl << "***** In RBF Test Case 3 *****" << endl;

    RC rc;
    string fileName = "test3";

    // Create a file named "test3"
    rc = pfm->createFile(fileName);
    assert(rc == success && "Creating the file should not fail.");

	rc = createFileShouldSucceed(fileName);
    assert(rc == success && "Creating the file failed.");

    // Open the file
    FileHandle fileHandle;
    rc = pfm->openFile(fileName, fileHandle);
    assert(rc == success && "Opening the file should not fail.");

    // Get the number of pages in the test file. In this case, it should be zero.
    unsigned count = fileHandle.getNumberOfPages();
    assert(count == (unsigned)0 && "The count should be zero at this moment.");

    // Close the file
    rc = pfm->closeFile(fileHandle);
    assert(rc == success && "Closing the file should not fail.");

    cout << "[PASS] Test Case 3 Passed!" << endl << endl;

    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
    
    remove("test3");
    
    RC rcmain = RBFTest_3(pfm);
    return rcmain;
}
