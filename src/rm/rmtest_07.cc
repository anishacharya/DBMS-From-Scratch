#include "test_util.h"

RC TEST_RM_7(const string &tableName)
{
    // Functions Tested
    // 1. Simple scan **
    cout << endl << "***** In RM Test Case 7 *****" << endl;

    RID rid;    
    int numTuples = 100;
    void *returnedData = malloc(200);

    set<int> ages; 
    RC rc = 0;
    for(int i = 0; i < numTuples; i++)
    {
        int age = 20+i;
        ages.insert(age);
    }

    // Set up the iterator
    RM_ScanIterator rmsi;
    string attr = "Age";
    vector<string> attributes;
    attributes.push_back(attr);
    rc = rm->scan(tableName, "", NO_OP, NULL, attributes, rmsi);
    assert(rc == success && "RelationManager::scan() should not fail.");
    int ageReturned = 0;
    
    while(rmsi.getNextTuple(rid, returnedData) != RM_EOF)
    {
        // cout << "Returned Age: " << *(int *)((char *)returnedData+1) << endl;
    	ageReturned = *(int *)((char *)returnedData+1);
        if (ages.find(ageReturned) == ages.end())
        {
            cout << "***** [FAIL] Test Case 7 Failed *****" << endl << endl;
            rmsi.close();
            free(returnedData);
            return -1;
        }
    }
    rmsi.close();
    
    // Delete a Table
    rc = rm->deleteTable(tableName);
    assert(rc == success && "RelationManager::deleteTable() should not fail.");

    free(returnedData);
    cout << "***** [PASS] Test Case 7 Passed *****" << endl << endl;
    return success;
}

int main()
{
     // Simple Scan
    RC rcmain = TEST_RM_7("tbl_employee3");

    return rcmain;
}
