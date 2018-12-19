#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_3C() {
	// Mandatory for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// 1. Filter -- IndexScan as input, on TypeReal attribute
	// SELECT * FROM RIGHT WHERE C >= 110.0
	cout << endl << "***** In QE Test Case 3c *****" << endl;

	RC rc = success;
	IndexScan *is = new IndexScan(*rm, "right", "C");
	float compVal = 110.0;
	float valueC = 0;

	// Set up condition
	Condition cond;
	cond.lhsAttr = "right.C";
	cond.op = GE_OP;
	cond.bRhsIsAttr = false;
	Value value;
	value.type = TypeReal;
	value.data = malloc(bufSize);
	*(float *) value.data = compVal;
	cond.rhsValue = value;

	int expectedResultCnt = 15; //110.00 ~ 124.00;
	int actualResultCnt = 0;

	// Create Filter
	Filter *filter = new Filter(is, cond);

	// Go over the data through iterator
	void *data = malloc(bufSize);

	bool nullBit = false;
	
	int valueB = 0;
	int valueD = 0;

	while (filter->getNextTuple(data) != QE_EOF) {
		int offset = 0;
		
		// is an attribute B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 7);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}
		valueB = *(int *)((char *)data+1+offset);

		// Print right.B
		cout << "right.B " << valueB;
		offset += sizeof(int);

		// is an attribute C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 6);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}
		valueC = *(float *)((char *)data+1+offset);
		
		// Print right.C
		cout << "  right.C " << valueC;
		offset += sizeof(float);
		if (valueC < compVal) {
			rc = fail;
			goto clean_up;
		}

		// is an attribute D NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 5);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}
		valueD = *(int *)((char *)data+1+offset);

		// Print right.D
		cout << "  right.D " << valueD << endl;
		offset += sizeof(int);

		memset(data, 0, bufSize);
		actualResultCnt++;
	}
	if (expectedResultCnt != actualResultCnt) {
		cout << "***** The number of returned tuple is not correct. *****" << endl;
		rc = fail;
	}

clean_up:
	delete filter;
	delete is;
	free(value.data);
	free(data);
	return rc;
}

int main() {

	if (testCase_3C() != success) {
		cout << "***** [FAIL] QE Test Case 3C failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 3C finished. The result will be examined. *****" << endl;
		return success;
	}
}
