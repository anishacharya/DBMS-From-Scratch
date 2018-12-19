#include "test_util.h"

RC TEST_RM_0(const string &tableName)
{
    // Functions Tested
    // 1. getAttributes **
    cout << endl << "***** In RM Test Case 0 *****" << endl;

    // GetAttributes
    vector<Attribute> attrs;
    RC rc = rm->getAttributes(tableName, attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    for(unsigned i = 0; i < attrs.size(); i++)
    {
        cout << (i+1) << ". Attr Name: " << attrs[i].name << " Attr Type: " << (AttrType) attrs[i].type << " Attr Len: " << attrs[i].length << endl;
    }

    cout << "***** [PASS] RM Test Case 0 passed *****" << endl;

    return success;
}

int main()
{
    // Get Attributes
    RC rcmain = TEST_RM_0("tbl_employee");

    return rcmain;
}

