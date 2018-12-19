#include "test_util.h"

RC TEST_RM_14(const string &tableName)
{
    // Functions Tested:
    // 1. Sysyem Catalog Implementation - Tables table
	cout << endl << "***** In RM Test Case 14 *****" << endl;

    // Get Catalog Attributes
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    // There should be at least three attributes: table-id, table-name, file-name
    if (attrs.size() < 3) {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed *****" << endl;
        return -1;
    } else if (attrs[0].name != "table-id" || attrs[1].name != "table-name" || attrs[2].name != "file-name") {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed *****" << endl;
        return -1;
    }

    RID rid;
    void *returnedData = malloc(200);

    // Set up the iterator
    RM_ScanIterator rmsi;
    vector<string> projected_attrs;
    for (int i = 0; i < attrs.size(); i++){
      projected_attrs.push_back(attrs[i].name);
    }

    rc = rm->scan(tableName, "", NO_OP, NULL, projected_attrs, rmsi);
    assert(rc == success && "RelationManager::scan() should not fail.");

    int count = 0;
    while(rmsi.getNextTuple(rid, returnedData) != RM_EOF)
    {
    	// We will manually check the returned tuples to see whether your implementation is correct or not.
        rm->printTuple(attrs, returnedData);
        count++;
    }
    rmsi.close();

    // There should be at least two rows - one for Tables and one for Columns
    if (count < 2) {
    	cout << "Your system catalog schema is not correct." << endl;
        cout << "***** [FAIL] Test Case 14 failed" << endl;
        free(returnedData);
        return -1;
    }

    // Deleting the catalog should fail.
    rc = rm->deleteTable(tableName);
    assert(rc != success && "RelationManager::deleteTable() on the system catalog table should fail.");

    free(returnedData);
    cout << "***** Test Case 14 Finished. We will manually check your implementation. *****"<<endl;
    return 0;
}

int main()
{
    // NOTE: your Tables table must be called "Tables"
    string catalog_table_name = "Tables";

    // Test Catalog Information
    RC rcmain = TEST_RM_14(catalog_table_name);

    return rcmain;
}
