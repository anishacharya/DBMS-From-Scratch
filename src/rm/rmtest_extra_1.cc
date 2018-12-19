#include "test_util.h"

RC RM_TEST_EXTRA_1(const string &tableName, const int nameLength, const string &name, const int age, const int height, const int salary)
{
	// Extra Test Case - Functions Tested:
    // 1. Insert tuple
    // 2. Read Attributes
    // 3. Drop Attributes **
    cout << endl << "***** In RM Extra Credit Test Case 1 *****" << endl;

    RID rid;
    int tupleSize = 0;
    void *tuple = malloc(200);
    void *returnedData = malloc(200);

    // Insert Tuple
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    prepareTuple(attrs.size(), nullsIndicator, nameLength, name, age, height, salary, tuple, &tupleSize);
    rc = rm->insertTuple(tableName, tuple, rid);
    assert(rc == success && "RelationManager::insertTuple() should not fail.");

    // Read Attribute
    rc = rm->readAttribute(tableName, rid, "Height", returnedData);
    assert(rc == success && "RelationManager::readAttribute() should not fail.");

    if(memcmp((char *)returnedData+nullAttributesIndicatorActualSize, (char *)tuple+14+nullAttributesIndicatorActualSize, 4) != 0)
    {
        cout << "RelationManager::readAttribute() failed." << endl;
        cout << "***** [FAIL] Extra Credit Test Case 1 Failed. *****"<<endl;
        free(returnedData);
        free(tuple);
        return -1;
    }
    else
    {
        // Drop the attribute
        rc = rm->dropAttribute(tableName, "Height");
        assert(rc == success && "RelationManager::dropAttribute() should not fail.");

        // Read Tuple and print the tuple
        rc = rm->readTuple(tableName, rid, returnedData);
        assert(rc == success && "RelationManager::readTuple() should not fail.");

        // Remove "Height" attribute from the Attribute vector
        attrs.erase(attrs.begin()+2);
        rc = rm->printTuple(attrs, returnedData);
        assert(rc == success && "RelationManager::printTuple() should not fail.");
    }

    free(tuple);
    free(returnedData);

    cout << "***** [PASS] Extra Credit Test Case 1 passed *****" << endl;
    return success;
}

int main()
{
    string name1 = "Peters";
    string name2 = "Victors";

    // Drop Attribute
    RC rcmain = createTable("tbl_employee100");
    rcmain = RM_TEST_EXTRA_1("tbl_employee100", 6, name1, 24, 170, 5000);

    return rcmain;
}
