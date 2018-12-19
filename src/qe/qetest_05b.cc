#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_5b() {
	// Mandatory for - grad team, grad solo, undergrad team
	// Optional for - undergrad solo (+5 extra credit points will be given based on the results of the BNLJ related tests)
	// Functions Tested
	// 1. BNLJoin -- on TypeInt Attribute
	// 2. Filter -- on TypeInt Attribute
	// SELECT * FROM left, right WHERE left.B = right.B AND right.B >= 100
	cout << endl << "***** In QE Test Case 5B *****" << endl;

	RC rc = success;

	// Prepare the iterator and condition
	TableScan *leftIn = new TableScan(*rm, "left");
	TableScan *rightIn = new TableScan(*rm, "right");

	Condition cond_j;
	cond_j.lhsAttr = "left.B";
	cond_j.op = EQ_OP;
	cond_j.bRhsIsAttr = true;
	cond_j.rhsAttr = "right.B";

	// Create NLJoin
	BNLJoin *bnlJoin = new BNLJoin(leftIn, rightIn, cond_j, 10);

	int compVal = 100;

	// Create Filter
	Condition cond_f;
	cond_f.lhsAttr = "right.B";
	cond_f.op = GE_OP;
	cond_f.bRhsIsAttr = false;
	Value value;
	value.type = TypeInt;
	value.data = malloc(bufSize);
	*(int *) value.data = compVal;
	cond_f.rhsValue = value;

	int expectedResultCnt = 10; // join result: [20,109] --> filter result [100, 109]
	int actualResultCnt = 0;
	int valueB = 0;

	Filter *filter = new Filter(bnlJoin, cond_f);

	// Go over the data through iterator
	void *data = malloc(bufSize);
	bool nullBit = false;
	
	while (filter->getNextTuple(data) != QE_EOF) {
		int offset = 0;

		// is an attribute left.A NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 7);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.A
		cout << "left.A " << *(int *) ((char *) data + offset + 1) << endl;
		offset += sizeof(int);

		// is an attribute left.B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 6);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.B
		valueB = *(int *) ((char *) data + offset + 1);
		cout << "left.B " << valueB << endl;
		offset += sizeof(int);
		if (valueB < 100 || valueB > 109) {
			rc = fail;
			goto clean_up;
		}

		// is an attribute left.C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 5);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.C
		cout << "left.C " << *(float *) ((char *) data + offset + 1) << endl;
		offset += sizeof(float);

		// is an attribute right.B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 4);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.B
		cout << "right.B " << *(int *) ((char *) data + offset + 1) << endl;
		offset += sizeof(int);

		// is an attribute right.C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 3);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.C
		cout << "right.C " << *(float *) ((char *) data + offset + 1) << endl;
		offset += sizeof(float);

		// is an attribute right.D NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 2);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.D
		cout << "right.D " << *(int *) ((char *) data + offset + 1) << endl;
		offset += sizeof(int);

		memset(data, 0, bufSize);
		++actualResultCnt;
	}

	if (expectedResultCnt != actualResultCnt) {
		cout << "***** The number of returned tuple is not correct. *****" << endl;
		rc = fail;
	}

clean_up:
	delete filter;
	delete bnlJoin;
	delete leftIn;
	delete rightIn;
	free(value.data);
	free(data);
	return rc;
}



int main() {

	if (testCase_5b() != success) {
		cout << "***** [FAIL] QE Test Case 5B failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 5B finished. The result will be examined. *****" << endl;
		return success;
	}
}
