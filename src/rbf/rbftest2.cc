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

int RBFTest_2(PagedFileManager *pfm)
{
    // Functions Tested:
    // 1. Destroy File
    cout << endl << "***** In RBF Test Case 2 *****" << endl;

    RC rc;
    string fileName = "test1";

    rc = pfm->destroyFile(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

	rc = destroyFileShouldSucceed(fileName);
    assert(rc == success  && "Destroying the file should not fail.");

    cout << "[PASS] Test Case 2 Passed!" << endl << endl;
    return 0;
}

int main()
{
	// To test the functionality of the paged file manager
    PagedFileManager *pfm = PagedFileManager::instance();
    
    // To test the functionality of the record-based file manager 
    // RecordBasedFileManager *rbfm = RecordBasedFileManager::instance(); 
    
    RC rcmain = RBFTest_2(pfm);
    return rcmain;
}
