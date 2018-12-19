#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qetest_util.h"

int testCase_1() {
	// Mandatory for all teams - grad team, grad solo, undergrad team, and undergrad solo
	// Create an Index
	// Load Data
	// Create an Index

	RC rc = success;
	cout << endl << "***** In QE Test Case 1 *****" << endl;
	rc = createIndexforLeftB();
	if (rc != success) {
		cout << "***** createIndexforLeftB() failed.  *****" << endl;
		return rc;
	}
	rc = populateLeftTable();
	if (rc != success) {
		cout << "***** populateLeftTable() failed.  *****" << endl;
		return rc;
	}
	rc = createIndexforLeftC();
	if (rc != success) {
		cout << "***** createIndexforLeftC() failed.  *****" << endl;
		return rc;
	}
	return rc;
}


int main() {

	// Initialize the system catalog
	if (deleteAndCreateCatalog() != success) {
		cout << "***** deleteAndCreateCatalog() failed." << endl;
		cout << "***** [FAIL] QE Test Case 1 failed. *****" << endl;
		return fail;
	}
	
	// Create the left table
	if (createLeftTable() != success) {
		cout << "***** createLeftTable() failed." << endl;
		cout << "***** [FAIL] QE Test Case 1 failed. *****" << endl;
		return fail;
	}

	if (testCase_1() != success) {
		cout << "***** [FAIL] QE Test Case 1 failed. *****" << endl;
		return fail;
	} else {
		cout << "***** [PASS] QE Test Case 1 passed. *****" << endl;
		return success;
	}
}
