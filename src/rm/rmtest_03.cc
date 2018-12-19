#include "test_util.h"

RC TEST_RM_3(const string &tableName, const int nameLength, const string &name, const int age, const float height, const int salary)
{
    // Functions Tested
    // 1. Insert Tuple    
    // 2. Update Tuple **
    // 3. Read Tuple
    cout << endl << "***** In RM Test Case 3****" << endl;
   
    RID rid; 
    int tupleSize = 0;
    int updatedTupleSize = 0;
    void *tuple = malloc(200);
    void *updatedTuple = malloc(200);
    void *returnedData = malloc(200);
   
    // Test Insert the Tuple
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    prepareTuple(attrs.size(), nullsIndicator, nameLength, name, age, height, salary, tuple, &tupleSize);
    rc = rm->insertTuple(tableName, tuple, rid);
    assert(rc == success && "RelationManager::insertTuple() should not fail.");
    cout << "Original RID:  " << rid.pageNum << " " << rid.slotNum << endl;

    // Test Update Tuple
    prepareTuple(attrs.size(), nullsIndicator, 6, "Newman", age, height, 100, updatedTuple, &updatedTupleSize);
    rc = rm->updateTuple(tableName, updatedTuple, rid);
    assert(rc == success && "RelationManager::updateTuple() should not fail.");

    // Test Read Tuple 
    rc = rm->readTuple(tableName, rid, returnedData);
    assert(rc == success && "RelationManager::readTuple() should not fail.");
   
    // Print the tuples 
    cout << "Inserted Data:" << endl;
    rm->printTuple(attrs, tuple);

    cout << "Updated data:" << endl;
    rm->printTuple(attrs, updatedTuple);

    cout << "Returned Data:" << endl;
    rm->printTuple(attrs, returnedData);
    
    if (memcmp(updatedTuple, returnedData, updatedTupleSize) == 0)
    {
        cout << "***** [PASS] RM Test Case 3 Passed *****" << endl << endl;
        free(tuple);
        free(updatedTuple);
        free(returnedData);
        return 0;
    }
    else
    {
        cout << "***** [FAIL] RM Test case 3 Failed *****" << endl << endl;
        free(tuple);
        free(updatedTuple);
        free(returnedData);
        return -1;
    }

}

int main()
{
    // Update Tuple
    RC rcmain = TEST_RM_3("tbl_employee", 6, "Thomas", 28, 187.3, 4000);

    return rcmain;
}
