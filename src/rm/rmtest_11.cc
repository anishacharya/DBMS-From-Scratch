#include "test_util.h"

RC TEST_RM_11(const string &tableName, vector<RID> &rids)
{
    // Functions Tested for large tables:
    // 1. delete tuple
    // 2. read tuple
    cout << endl << "***** In RM Test Case 11 *****" << endl;

    int numTuples = 2000;
    RC rc = 0;
    void * returnedData = malloc(2000);
    
    readRIDsFromDisk(rids, numTuples);

    // Delete the first 1000 tuples
    for(int i = 0; i < 1000; i++)
    {
        rc = rm->deleteTuple(tableName, rids[i]);
        assert(rc == success && "RelationManager::deleteTuple() should not fail.");
    }

    // Try to read the first 1000 deleted tuples
    for(int i = 0; i < 1000; i++)
    {
		rc = rm->readTuple(tableName, rids[i], returnedData);
		assert(rc != success && "RelationManager::readTuple() on a deleted tuple should fail.");
    }

    for(int i = 1000; i < 2000; i++)
    {
        rc = rm->readTuple(tableName, rids[i], returnedData);
        assert(rc == success && "RelationManager::readTuple() should not fail.");
    }
    cout << "***** [PASS] Test Case 11 Passed *****" << endl << endl;

    free(returnedData);
    
    return success;
}

int main()
{
    vector<RID> rids;
    vector<int> sizes;

	// Delete Tuple
    RC rcmain = TEST_RM_11("tbl_employee4", rids);

    return rcmain;
}
