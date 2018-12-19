#include "test_util.h"

RC TEST_RM_5(const string &tableName, const int nameLength, const string &name, const int age, const float height, const int salary)
{
    // Functions Tested
    // 0. Insert tuple;
    // 1. Read Tuple
    // 2. Delete Table **
    // 3. Read Tuple
    cout << endl << "***** In RM Test Case 5 *****" << endl;
   
    RID rid; 
    int tupleSize = 0;
    void *tuple = malloc(200);
    void *returnedData = malloc(200);
    void *returnedData1 = malloc(200);
   
    // Test Insert Tuple
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    prepareTuple(attrs.size(), nullsIndicator, nameLength, name, age, height, salary, tuple, &tupleSize);
    rc = rm->insertTuple(tableName, tuple, rid);
    assert(rc == success && "RelationManager::insertTuple() should not fail.");

    // Test Read Tuple 
    rc = rm->readTuple(tableName, rid, returnedData);
    assert(rc == success && "RelationManager::readTuple() should not fail.");

    // Test Delete Table
    rc = rm->deleteTable(tableName);
    assert(rc == success && "RelationManager::deleteTable() should not fail.");
    
    // Test Read Tuple 
    memset((char*)returnedData1, 0, 200);
    rc = rm->readTuple(tableName, rid, returnedData1);
    assert(rc != success && "RelationManager::readTuple() on a deleted table should fail.");
    
    if(memcmp(returnedData, returnedData1, tupleSize) != 0)
    {
        cout << "***** [PASS] Test Case 5 Passed *****" << endl << endl;
        free(tuple);
        free(returnedData);
        free(returnedData1);
        return success;
    }
    else
    {
        cout << "***** [FAIL] Test Case 5 Failed *****" << endl << endl;
        free(tuple);
        free(returnedData);
        free(returnedData1);
        return -1;
    }
}

int main()
{
    // Delete Table
    RC rcmain = TEST_RM_5("tbl_employee", 6, "Martin", 26, 173.6, 8000);

    return rcmain;
}
