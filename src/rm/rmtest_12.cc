#include "test_util.h"

RC TEST_RM_12(const string &tableName)
{
    // Functions Tested for large tables
    // 1. scan
    cout << endl << "***** In RM Test case 12 *****" << endl;

    RM_ScanIterator rmsi;
    vector<string> attrs;
    attrs.push_back("attr5");
    attrs.push_back("attr12");
    attrs.push_back("attr28");
   
    RC rc = rm->scan(tableName, "", NO_OP, NULL, attrs, rmsi); 
    assert(rc == success && "RelationManager::scan() should not fail.");

    RID rid;
    int j = 0;
    void *returnedData = malloc(2000);

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());

    while(rmsi.getNextTuple(rid, returnedData) != RM_EOF)
    {
        if(j % 200 == 0)
        {
            int offset = 0;

            cout << "Real Value: " << *(float *)((char *)returnedData+nullAttributesIndicatorActualSize) << endl;
            offset += 4;
        
            int size = *(int *)((char *)returnedData + offset + nullAttributesIndicatorActualSize);
            cout << "String size: " << size << endl;
            offset += 4;

            char *buffer = (char *)malloc(size + 1);
            memcpy(buffer, (char *)returnedData + offset + nullAttributesIndicatorActualSize, size);
            buffer[size] = 0;
            offset += size;
    
            cout << "VarChar Value: " << buffer << endl;

            cout << "Integer Value: " << *(int *)((char *)returnedData + offset + nullAttributesIndicatorActualSize) << endl << endl;
            offset += 4;

            free(buffer);
        }
        j++;
        memset(returnedData, 0, 2000);
    }
    rmsi.close();
    cout << "Total number of tuples: " << j << endl << endl;
    if (j > 1000) {
        cout << "***** [FAIL] Test Case 12 Failed *****" << endl << endl;
        free(returnedData);
        return -1;
    }

    cout << "***** [PASS] Test Case 12 Passed *****" << endl << endl;
    free(returnedData);

    return success;
}

int main()
{
	// Scan
    RC rcmain = TEST_RM_12("tbl_employee4");
    
    return rcmain;
}
