#include "test_util.h"

RC TEST_RM_8(const string &tableName, vector<RID> &rids, vector<int> &sizes)
{
    // Functions Tested for large tables:
    // 1. getAttributes
    // 2. insert tuple
    cout << endl << "***** In RM Test Case 8 *****" << endl;

    RID rid; 
    void *tuple = malloc(2000);
    int numTuples = 2000;

    // GetAttributes
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

//    for(unsigned i = 0; i < attrs.size(); i++)
//    {
//        cout << "Attr Name: " << attrs[i].name << "  Type: " << (AttrType) attrs[i].type << " Len: " << attrs[i].length << endl;
//    }

    // Insert 2000 tuples into table
    for(int i = 0; i < numTuples; i++)
    {
        // Test insert Tuple
        int size = 0;
        memset(tuple, 0, 2000);
        prepareLargeTuple(attrs.size(), nullsIndicator, i, tuple, &size);

        rc = rm->insertTuple(tableName, tuple, rid);
        assert(rc == success && "RelationManager::insertTuple() should not fail.");

        rids.push_back(rid);
        sizes.push_back(size);        
    }
    cout << "***** [PASS] Test Case 8 Passed *****" << endl << endl;
    free(tuple);
    writeRIDsToDisk(rids);
    writeSizesToDisk(sizes);

    return success;
}

int main()
{
    vector<RID> rids;
    vector<int> sizes;

	// Insert Tuple
    RC rcmain = TEST_RM_8("tbl_employee4", rids, sizes);

    return rcmain;
}
