#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_8B() {
	// Mandatory for the grad teams
	// Optional for the other teams - grad solo, undergrad team, and undergrad solo
	// (+5 extra credit points will be given based on the results of the basic aggregation related tests)
	// 1. Basic aggregation - AVG
	// SELECT AVG(right.B) from left

    cout << "***** In QE Test Case 8B *****" << endl;

    // Create TableScan
    TableScan *input = new TableScan(*rm, "right");

    RC rc = success;

    // Create Aggregate
    Attribute aggAttr;
    aggAttr.name = "right.B";
    aggAttr.type = TypeInt;
    aggAttr.length = 4;
    Aggregate *agg = new Aggregate(input, aggAttr, AVG);

    void *data = malloc(bufSize);
    float average = 0.0;
	int count = 0;
	    
    while(agg->getNextTuple(data) != QE_EOF)
    {
    	average = *(float *) ((char *) data + 1);
        cout << "AVG(right.B) " << average << endl;
        memset(data, 0, sizeof(float)+1);
        count++;
        if (count > 1) {
        	cout << "***** The number of returned tuple is not correct. *****" << endl;
        	rc = fail;
        	break;
        }
    }

    if (average != 69.5) {
    	rc = fail;
    }

    delete agg;
    delete input;
    free(data);
    return rc;
}


int main() {
	
	if (testCase_8B() != success) {
		cout << "***** [FAIL] QE Test Case 8B failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 8B finished. The result will be examined. *****" << endl;
		return success;
	}
}
