#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_6() {
	// Mandatory for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// 1. INLJoin -- on TypeReal Attribute
	// SELECT * from left, right WHERE left.C = right.C
	cout << endl << "***** In QE Test Case 6 *****" << endl;

	RC rc = success;

	// Prepare the iterator and condition
	TableScan *leftIn = new TableScan(*rm, "left");
	IndexScan *rightIn = new IndexScan(*rm, "right", "C");

	Condition cond;
	cond.lhsAttr = "left.C";
	cond.op = EQ_OP;
	cond.bRhsIsAttr = true;
	cond.rhsAttr = "right.C";

	int expectedResultCnt = 75; // 50.0~124.0  left.C: [50.0,149.0], right.C: [25.0,124.0]
	int actualResultCnt = 0;
	float valueC = 0;

	// Create INLJoin
	INLJoin *inlJoin = new INLJoin(leftIn, rightIn, cond);

	// Go over the data through iterator
	void *data = malloc(bufSize);
	bool nullBit = false;
	
	while (inlJoin->getNextTuple(data) != QE_EOF) {
		int offset = 0;

		// is an attribute left.A NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 7);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.A
		cout << "left.A " << *(int *) ((char *) data + offset + 1);
		offset += sizeof(int);

		// is an attribute left.B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 6);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.B
		cout << "  left.B " << *(int *) ((char *) data + offset + 1);
		offset += sizeof(int);

		// is an attribute left.C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 5);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print left.C
		cout << "  left.C " << *(float *) ((char *) data + offset + 1);
		offset += sizeof(float);

		// is an attribute right.B NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 5);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.B
		cout << "  right.B " << *(int *) ((char *) data + offset + 1);
		offset += sizeof(int);

		// is an attribute right.C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 4);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.C
		valueC = *(float *) ((char *) data + offset + 1);
		cout << "  right.C " << valueC;
		offset += sizeof(float);
		if (valueC < 50.0 || valueC > 124.0) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			rc = fail;
			goto clean_up;
		}

		// is an attribute right.C NULL?
		nullBit = *(unsigned char *)((char *)data) & (1 << 3);
		if (nullBit) {
			cout << endl << "***** A returned value is not correct. *****" << endl;
			goto clean_up;
		}
		// Print right.D
		cout << "  right.D " << *(int *) ((char *) data + offset + 1) << endl;
		offset += sizeof(int);

		memset(data, 0, bufSize);
		actualResultCnt++;
	}

	if (expectedResultCnt != actualResultCnt) {
		cout << "***** The number of returned tuple is not correct. *****" << endl;
		rc = fail;
	}

clean_up:
	delete inlJoin;
	delete leftIn;
	delete rightIn;
	free(data);
	return rc;
}

int main() {

	if (testCase_6() != success) {
		cout << "***** [FAIL] QE Test Case 6 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 6 finished. The result will be examined. *****" << endl;
		return success;
	}
}
