#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"


RC testCase_3() {
	// Mandatory for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// Filter -- TableScan as input, on an Integer Attribute
	// SELECT * FROM LEFT WHERE B <= 30
	cout << endl << "***** In QE Test Case 3 *****" << endl;
	RC rc = success;

	TableScan *ts = new TableScan(*rm, "left");
	int compVal = 30;
	int valueB = 0;

	// Set up condition
	Condition cond;
	cond.lhsAttr = "left.B";
	cond.op = LE_OP;
	cond.bRhsIsAttr = false;
	Value value;
	value.type = TypeInt;
	value.data = malloc(bufSize);
	*(int *) value.data = compVal;
	cond.rhsValue = value;

	int expectedResultCnt = 21; //10~30;
	int actualResultCnt = 0;

	// Create Filter
	Filter *filter = new Filter(ts, cond);

	// Go over the data through iterator
	void *data = malloc(bufSize);
	bool nullBit = false;
	
	int valueA = 0;
	float valueC = 0.0;
	
	while (filter->getNextTuple(data) != QE_EOF) {
		int offset = 0;
		// Print left.A
		// Null indicators should be placed in the beginning.

		// is an attribute A NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 7);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}
		valueA = *(int *)((char *)data+1+offset);

		cout << "left.A " << valueA;
		offset += sizeof(int);

		// is an attribute B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 6);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}

		// Print left.B
		valueB = *(int *)((char *)data+1+offset);
		cout << "  left.B " << valueB;
		offset += sizeof(int);
		if (valueB > compVal) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}


		// is an attribute C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 5);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}
		valueC = *(float *)((char *)data+1+offset);
		
		// Print left.C
		cout << "  left.C " << valueC << endl;
		offset += sizeof(float);

		memset(data, 0, bufSize);
		actualResultCnt++;
	}

	if (expectedResultCnt != actualResultCnt) {
		cout << "***** The number of returned tuple is not correct. *****" << endl;
		rc = fail;
	}

clean_up:
	delete filter;
	delete ts;
	free(value.data);
	free(data);
	return rc;
}


int main() {

	if (testCase_3() != success) {
		cout << "***** [FAIL] QE Test Case 3 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 3 finished. The result will be examined. *****" << endl;
		return success;
	}
}
