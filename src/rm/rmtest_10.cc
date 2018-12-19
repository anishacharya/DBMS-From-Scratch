#include "test_util.h"

RC TEST_RM_10(const string &tableName, vector<RID> &rids, vector<int> &sizes)
{
    // Functions Tested for large tables:
    // 1. update tuple
    // 2. read tuple
    cout << endl << "***** In RM Test case 10 *****" << endl;

    int numTuples = 2000;
    void *tuple = malloc(2000);
    void *returnedData = malloc(2000);
    
    readRIDsFromDisk(rids, numTuples);
    readSizesFromDisk(sizes, numTuples);

    // GetAttributes
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

    // Update the first 1000 tuples
    int size = 0;
    for(int i = 0; i < 1000; i++)
    {
        memset(tuple, 0, 2000);
        RID rid = rids[i];

        prepareLargeTuple(attrs.size(), nullsIndicator, i+10, tuple, &size);
        rc = rm->updateTuple(tableName, tuple, rid);
        assert(rc == success && "RelationManager::updateTuple() should not fail.");

        sizes[i] = size;
        rids[i] = rid;
    }

    // Read the updated records and check the integrity
    for(int i = 0; i < 1000; i++)
    {
        memset(tuple, 0, 2000);
        memset(returnedData, 0, 2000);
        prepareLargeTuple(attrs.size(), nullsIndicator, i+10, tuple, &size);
        rc = rm->readTuple(tableName, rids[i], returnedData);
        assert(rc == success && "RelationManager::readTuple() should not fail.");

        if(memcmp(returnedData, tuple, sizes[i]) != 0)
        {
            cout << "***** [FAIL] Test Case 10 Failed *****" << endl << endl;
            free(tuple);
            free(returnedData);
            return -1;
        }
    }

    free(tuple);
    free(returnedData);

    cout << "***** [PASS] Test Case 10 Passed *****" << endl << endl;

    return success;

}

int main()
{
    vector<RID> rids;
    vector<int> sizes;

	 // Update Tuple
    RC rcmain = TEST_RM_10("tbl_employee4", rids, sizes);

    return rcmain;
}
