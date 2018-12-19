#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"


RC testCase_9B()
{
	// Optional for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// (+5 extra credit points will be given based on the results of the group-based hash aggregation related tests)
    // Aggregate -- SUM (with GroupBy)
	// SELECT group.B, SUM(group.A) FROM group GROUP BY group.B

    cout << "***** In QE Test Case 9B *****" << endl;

    RC rc = success;

    // Create TableScan
    TableScan *input = new TableScan(*rm, "group");

    // Create Aggregate
    Attribute aggAttr;
    aggAttr.name = "group.A";
    aggAttr.type = TypeInt;
    aggAttr.length = 4;

    Attribute gAttr;
    gAttr.name = "group.B";
    gAttr.type = TypeInt;
    gAttr.length = 4;
    Aggregate *agg = new Aggregate(input, aggAttr, gAttr, SUM);

    int idVal = 0;
    int sumVal = 0;
    int expectedResultCnt = 5;
    int actualResultCnt = 0;

    void *data = malloc(bufSize);
    while(agg->getNextTuple(data) != QE_EOF)
    {
        int offset = 0;

        // Print group.B
        idVal = *(int *)((char *)data + offset + 1);
        cout << "group.B " << idVal;
        offset += sizeof(int);

        // Print SUM(group.A)
        sumVal = *(float *)((char *)data + offset + 1);
        cout << "  SUM(group.A) " <<  sumVal << endl;
        offset += sizeof(float);

        memset(data, 0, bufSize);
        if (sumVal != (idVal*20)) {
            cout << "***** The group-based aggregation is not working properly. *****" << endl;
        	rc = fail;
        	goto clean_up;
        }
        actualResultCnt++;
    }

    if (expectedResultCnt != actualResultCnt) {
        cout << "***** The number of returned tuple is not correct. *****" << endl;
    	rc = fail;
    }

clean_up:
	delete agg;
	delete input;
    free(data);
    return rc;
}


int main() {
	
	if (testCase_9B() != success) {
		cout << "***** [FAIL] QE Test Case 9B failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 9B finished. The result will be examined. *****" << endl;
		return success;
	}
}
