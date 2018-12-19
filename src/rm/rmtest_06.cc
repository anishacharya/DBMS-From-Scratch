#include "test_util.h"

RC TEST_RM_6(const string &tableName)
{
    // Functions Tested
    // 1. Simple scan **
    cout << endl << "***** In RM Test Case 6 *****" << endl;

    RID rid;    
    int tupleSize = 0;
    int numTuples = 100;
    void *tuple;
    void *returnedData = malloc(200);

    // Test Insert Tuple
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    RID rids[numTuples];
    set<int> ages; 
    for(int i = 0; i < numTuples; i++)
    {
        tuple = malloc(200);

        // Insert Tuple
        float height = (float)i;
        int age = 20+i;
        prepareTuple(attrs.size(), nullsIndicator, 6, "Tester", age, height, 123, tuple, &tupleSize);
        ages.insert(age);
        rc = rm->insertTuple(tableName, tuple, rid);
        assert(rc == success && "RelationManager::insertTuple() should not fail.");

        rids[i] = rid;
    }

    // Set up the iterator
    RM_ScanIterator rmsi;
    string attr = "Age";
    vector<string> attributes;
    attributes.push_back(attr);
    rc = rm->scan(tableName, "", NO_OP, NULL, attributes, rmsi);
    assert(rc == success && "RelationManager::scan() should not fail.");
    
    nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attributes.size());
    while(rmsi.getNextTuple(rid, returnedData) != RM_EOF)
    {
        // cout << "Returned Age: " << *(int *)((char *)returnedData+nullAttributesIndicatorActualSize) << endl;
        if (ages.find(*(int *)((char *)returnedData+nullAttributesIndicatorActualSize)) == ages.end())
        {
            cout << "***** [FAIL] Test Case 6 Failed *****" << endl << endl;
            rmsi.close();
            free(returnedData);
            return -1;
        }
    }
    rmsi.close();

    free(returnedData);
    cout << "***** [PASS] Test Case 6 Passed *****" << endl << endl;
    return 0;
}

int main()
{
     // Simple Scan
    RC rcmain = TEST_RM_6("tbl_employee3");

    return rcmain;
}
