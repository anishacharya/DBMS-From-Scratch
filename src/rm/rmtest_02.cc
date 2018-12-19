#include "test_util.h"

RC TEST_RM_2(const string &tableName, const int nameLength, const string &name, const int age, const float height, const int salary)
{
    // Functions Tested
    // 1. Insert tuple
    // 2. Delete Tuple **
    // 3. Read Tuple
    cout << endl << "***** In RM Test Case 2 *****" << endl;
   
    RID rid; 
    int tupleSize = 0;
    void *tuple = malloc(200);
    void *returnedData = malloc(200);

    // Test Insert the Tuple
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    prepareTuple(attrs.size(), nullsIndicator, nameLength, name, age, height, salary, tuple, &tupleSize);
    cout << "The tuple to be inserted:" << endl;
    rc = rm->printTuple(attrs, tuple);
    rc = rm->insertTuple(tableName, tuple, rid);
    assert(rc == success && "RelationManager::insertTuple() should not fail.");
    
    // Delete the tuple
    rc = rm->deleteTuple(tableName, rid);
    assert(rc == success && "RelationManager::deleteTuple() should not fail.");
    
    // Read Tuple after deleting it - should fail
    memset(returnedData, 0, 200);
    rc = rm->readTuple(tableName, rid, returnedData);
cout<<"FAILED IN deleteRecord" <<endl;
    assert(rc != success && "Reading a deleted tuple should fail.");
    
    // Compare the two memory blocks to see whether they are different
    if (memcmp(tuple, returnedData, tupleSize) != 0)
    {   
        cout << "***** [PASS] RM Test Case 2 Passed *****" << endl << endl;
        free(tuple);
        free(returnedData);
        return success;
    }
    else
    {
        cout << "***** [FAIL] RM Test case 2 failed *****" << endl << endl;
        free(tuple);
        free(returnedData);
        return -1;
    }
        
}

int main()
{
    // Delete Tuple
    RC rcmain = TEST_RM_2("tbl_employee", 6, "Victor", 22, 180.2, 6000);

    return rcmain;
}
