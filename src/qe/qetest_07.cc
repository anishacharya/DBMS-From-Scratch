#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_7() {
	// Optional for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// (+10 extra credit points will be given based on the results of the GHJ related tests)
	// 1. GHJoin -- on TypeReal Attribute
	// SELECT * from largeleft, largeright WHERE largeleft.C = largeright.C
	cout << endl << "***** In QE Test Case 7 *****" << endl;

	RC rc = success;

	// Prepare the iterator and condition
	TableScan *leftIn = new TableScan(*rm, "largeleft");
	TableScan *rightIn = new TableScan(*rm, "largeright");

	Condition cond;
	cond.lhsAttr = "largeleft.C";
	cond.op = EQ_OP;
	cond.bRhsIsAttr = true;
	cond.rhsAttr = "largeright.C";

	int expectedResultCnt = 49975; // 50.0~50024.0  left.C: [50.0,50049.0], right.C: [25.0,50024.0]
	int actualResultCnt = 0;
	float valueC = 0;
	int numPartitons = 10;
	
	// Create GHJoin
	GHJoin *ghJoin = new GHJoin(leftIn, rightIn, cond, numPartitons);

	// Go over the data through iterator
	void *data = malloc(bufSize);
	bool nullBit = false;
	
	while (ghJoin->getNextTuple(data) != QE_EOF) {

		// At this point, partitions should be on disk.

		if (actualResultCnt % 5000 == 0) {
			cout << "Processing " << actualResultCnt << " of " << largeTupleCount << " tuples." << endl;
			int offset = 0;
			// is an attribute left.A NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 7);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print left.A
			cout << "largeleft.A " << *(int *) ((char *) data + offset + 1);
			offset += sizeof(int);

			// is an attribute left.B NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 6);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print left.B
			cout << "  largeleft.B " << *(int *) ((char *) data + offset + 1);
			offset += sizeof(int);

			// is an attribute left.C NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 5);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print left.C
			cout << "  largeleft.C " << *(float *) ((char *) data + offset + 1);
			offset += sizeof(float);

			// is an attribute right.B NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 5);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print right.B
			cout << "  largeright.B " << *(int *) ((char *) data + offset + 1);
			offset += sizeof(int);

			// is an attribute right.C NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 4);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print right.C
			valueC = *(float *) ((char *) data + offset + 1);
			cout << "  largeright.C " << valueC;
			offset += sizeof(float);
			if (valueC < 50.0 || valueC > 50024.0) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				rc = fail;
				goto clean_up;
			}

			// is an attribute right.D NULL?
			nullBit = *(unsigned char *)((char *)data) & (1 << 3);
			if (nullBit) {
				cout << endl << "***** A returned value is not correct. *****" << endl;
				goto clean_up;
			}
			// Print right.D
			cout << "  largeright.D " << *(int *) ((char *) data + offset + 1) << endl;
			offset += sizeof(int);
		}

		memset(data, 0, bufSize);
		actualResultCnt++;

	}

	if (expectedResultCnt != actualResultCnt) {
		cout << "***** The number of returned tuple is not correct. *****" << endl;
		rc = fail;
	}

clean_up:
	delete ghJoin;
	delete leftIn;
	delete rightIn;
	free(data);
	return rc;
}

int main() {

	// Create left/right large table, and populate the table
	if (createLargeLeftTable() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	}

	if (populateLargeLeftTable() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	}

	if (createLargeRightTable() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	}

	if (populateLargeRightTable() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	}
	
	if (testCase_7() != success) {
		cout << "***** [FAIL] QE Test Case 7 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** QE Test Case 7 finished. The result will be examined. *****" << endl;
		return success;
	}
}
