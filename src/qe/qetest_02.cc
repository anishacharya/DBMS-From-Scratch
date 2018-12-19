#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

RC testCase_2() {
	// Mandatory for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// Create an Index
	// Load Data
	// Create an Index
	
	RC rc = success;
	cout << endl << "***** In QE Test Case 2 *****" << endl;
	rc = createIndexforRightB();
	if (rc != success) {
		cout << "***** createIndexforRightB() failed.  *****" << endl;
		return rc;
	}
	rc = populateRightTable();
	if (rc != success) {
		cout << "***** populateRightTable() failed.  *****" << endl;
		return rc;
	}
	rc = createIndexforRightC();
	if (rc != success) {
		cout << "***** createIndexforRightC() failed.  *****" << endl;
		return rc;
	}
	return rc;
}

int main() {

	// Create the right table
	if (createRightTable() != success) {
		cout << "***** createRightTable() failed. *****" << endl;
		cout << "***** [FAIL] QE Test Case 2 failed. *****" << endl;
		return fail;
	}

	if (testCase_2() != success) {
		cout << "***** [FAIL] QE Test Case 2 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** [PASS] QE Test Case 2 passed. *****" << endl;
		return success;
	}
}
