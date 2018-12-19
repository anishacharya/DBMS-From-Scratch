#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_5() {
	// Mandatory for - grad team, grad solo, undergrad team
	// Optional for - undergrad solo (+5 extra credit points will be given based on the results of the BNLJ related tests)
	// 1. BNLJoin -- on TypeInt Attribute
	// SELECT * FROM left, right where left.B = right.B
	cout << endl << "***** In QE Test Case 5 *****" << endl;

	RC rc = success;
	// Prepare the iterator and condition
	TableScan *leftIn = new TableScan(*rm, "left");
	TableScan *rightIn = new TableScan(*rm, "right");

	Condition cond;
	cond.lhsAttr = "left.B";
	cond.op = EQ_OP;
	cond.bRhsIsAttr = true;
	cond.rhsAttr = "right.B";

	int expectedResultCnt = 90; //20~109 --> left.B: [10,109], right.B: [20,119]
	int actualResultCnt = 0;
	int valueB = 0;

	// Create BNLJoin
	BNLJoin *bnlJoin = new BNLJoin(leftIn, rightIn, cond, 10);


	// Go over the data through iterator
	void *data = malloc(bufSize);
	bool nullBit = false;
	
	while (bnlJoin->getNextTuple(data) != QE_EOF) {
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
		cout << "left.B " << *(int *) ((char *) data + offset + 1) << endl;
		offset += sizeof(int);

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
		valueB =  *(int *) ((char *) data + offset + 1);
		cout << "right.B " << valueB << endl;
		offset += sizeof(int);

		if (valueB < 20 || valueB > 109) {
			rc = fail;
			goto clean_up;
		}

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
	delete bnlJoin;
	delete leftIn;
	delete rightIn;
	free(data);
	return rc;
}

int main() {

	if (testCase_5() != success) {
		cout << "***** [FAIL] QE Test Case 5 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 5 finished. The result will be examined. *****" << endl;
		return success;
	}
}
