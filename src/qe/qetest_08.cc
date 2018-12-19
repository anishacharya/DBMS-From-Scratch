#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_8() {
	// Mandatory for the grad teams
	// Optional for the other teams - grad solo, undergrad team, and undergrad solo
	// (+5 extra credit points will be given based on the results of the basic aggregation related tests)
	// 1. Basic aggregation - max
	// SELECT max(left.B) from left
    cout << "***** In QE Test Case 8 *****" << endl;

	RC rc = success;

    // Create TableScan
    TableScan *input = new TableScan(*rm, "left");

    // Create Aggregate
    Attribute aggAttr;
    aggAttr.name = "left.B";
    aggAttr.type = TypeInt;
    aggAttr.length = 4;
    Aggregate *agg = new Aggregate(input, aggAttr, MAX);

	int count = 0;
    void *data = malloc(bufSize);
    
    // An aggregation returns a float value
    float maxVal = 0.0;
	
    while(agg->getNextTuple(data) != QE_EOF)
    {
    	maxVal = *(float *) ((char *) data + 1);
        cout << "MAX(left.B) " << maxVal << endl;
        memset(data, 0, sizeof(int));
        count++;
        if (count > 1) {
        	cout << "***** The number of returned tuple is not correct. *****" << endl;
        	rc = fail;
        	break;
        }
    }

    if (maxVal != 109.0) {
    	rc = fail;
    }

    delete agg;
    delete input;
    free(data);
    return rc;

}


int main() {
	
	if (testCase_8() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 7 finished. The result will be examined. *****" << endl;
		return success;
	}
}
