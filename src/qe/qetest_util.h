#ifndef _qetest_util_h_
#define _qetest_util_h_

// #ifndef _success_
// #define _success_
// const int success = 0;
// #endif
// 
#ifndef _fail_
#define _fail_
const int fail = -1;
#endif

#include <fstream>
#include <iostream>

#include <vector>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "qe.h"
#include "../rm/test_util.h"

// Global Initialization
// RelationManager *rm = RelationManager::instance();
IndexManager *im = IndexManager::instance();

// Number of tuples in each relation
const int tupleCount = 100;

// Number of tuples in left large relation
const int varcharTupleCount = 1000;

// Number of tuples in large relation
const int largeTupleCount = 50000;

// Buffer size and character buffer size
const unsigned bufSize = 200;

int createLeftTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Left Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "A";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "B";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("left", attrs);
	if (rc == success) {
		cout << "****Left Table Created!****" << endl;
	}
	return rc;
}

int createLargeLeftTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Large Left Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "A";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "B";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("largeleft", attrs);
	if (rc == success) {
		cout << "****Large left Table Created!****" << endl;
	}
	return rc;
}


int createLeftVarCharTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Left Large Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "A";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "B";
	attr.type = TypeVarChar;
	attr.length = 30;
	attrs.push_back(attr);

	RC rc = rm->createTable("leftvarchar", attrs);
	if (rc == success) {
		cout << "****Left Var Char Table Created!****" << endl;
	}
	return rc;
}

int createRightTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Right Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "B";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "D";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("right", attrs);
	if (rc == success) {
		cout << "****Right Table Created!****" << endl;
	}
	return rc;
}

int createLargeRightTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Large Right Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "B";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "D";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("largeright", attrs);
	if (rc == success) {
		cout << "****Large Right Table Created!****" << endl;
	}
	return rc;
}

int createRightVarCharTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Right Large Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "B";
	attr.type = TypeVarChar;
	attr.length = 30;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("rightvarchar", attrs);
	if (rc == success) {
		cout << "****Right Var Char Table Created!****" << endl;
	}
	return rc;
}

int createGroupTable() {
	// Functions Tested;
	// 1. Create Table
	cout << "****Create Group Table****" << endl;

	vector<Attribute> attrs;

	Attribute attr;
	attr.name = "A";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "B";
	attr.type = TypeInt;
	attr.length = 4;
	attrs.push_back(attr);

	attr.name = "C";
	attr.type = TypeReal;
	attr.length = 4;
	attrs.push_back(attr);

	RC rc = rm->createTable("group", attrs);
	if (rc == success) {
		cout << "****Group Table Created!****" << endl;
	}
	return rc;
}

// Prepare the tuple to left table in the format conforming to Insert/Update/ReadTuple and readAttribute
void prepareLeftTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int a, const int b, const float c, void *buf) {
	int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attributeCount);

	// Null-indicator for the fields
    memcpy((char *)buf + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
	offset += nullAttributesIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a tuple consists of four attributes and they are all nulls, then the bit representation will be: [11110000]

	// Is the A field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 7);

	if (!nullBit) {
		memcpy((char *) buf + offset, &a, sizeof(int));
		offset += sizeof(int);
	}
	
	// Is the B field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 6);

	if (!nullBit) {
		memcpy((char *) buf + offset, &b, sizeof(int));
		offset += sizeof(int);
	}

	// Is the C field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 5);

	if (!nullBit) {
		memcpy((char *) buf + offset, &c, sizeof(float));
		offset += sizeof(float);
	}
}

// Prepare the tuple to right table in the format conforming to Insert/Update/ReadTuple, readAttribute
void prepareRightTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int b, const float c, const int d, void *buf) {
	int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attributeCount);

	// Null-indicator for the fields
    memcpy((char *)buf + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
	offset += nullAttributesIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a tuple consists of four attributes and they are all nulls, then the bit representation will be: [11110000]

	// Is the B field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 7);

	if (!nullBit) {
		memcpy((char *) buf + offset, &b, sizeof(int));
		offset += sizeof(int);
	}

	// Is the C field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 6);

	if (!nullBit) {
		memcpy((char *) buf + offset, &c, sizeof(float));
		offset += sizeof(float);
	}
	

	// Is the C field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 5);
	
	if (!nullBit) {
		memcpy((char *) buf + offset, &d, sizeof(int));
		offset += sizeof(int);
	}
	
}

// Prepare the tuple to left var char table in the format conforming to Insert/Update/ReadTuple and readAttribute
void prepareLeftVarCharTuple(int attributeCount, unsigned char *nullAttributesIndicator, int a, int length, const string b, void *buf) {
	int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attributeCount);

	// Null-indicator for the fields
    memcpy((char *)buf + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
	offset += nullAttributesIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a tuple consists of four attributes and they are all nulls, then the bit representation will be: [11110000]

	// Is the A field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 7);
	if (!nullBit) {
		memcpy((char *) buf + offset, &a, sizeof(int));
		offset += sizeof(int);
	}
	
	// Is the B field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 6);
	if (!nullBit) {
		memcpy((char *) buf + offset, &length, sizeof(int));
		offset += sizeof(int);
		memcpy((char *) buf + offset, b.c_str(), length);
		offset += length;
	}
	
}

// Prepare the tuple to right var char table in the format conforming to Insert/Update/ReadTuple and readAttribute
void prepareRightVarCharTuple(int attributeCount, unsigned char *nullAttributesIndicator, int length, const string b, float c, void *buf) {
	int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attributeCount);

	// Null-indicator for the fields
    memcpy((char *)buf + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
	offset += nullAttributesIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a tuple consists of four attributes and they are all nulls, then the bit representation will be: [11110000]

	// Is the B field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 7);
	if (!nullBit) {
		memcpy((char *) buf + offset, &length, sizeof(int));
		offset += sizeof(int);
		memcpy((char *) buf + offset, b.c_str(), length);
		offset += length;
	}
	
	// Is the C field not-NULL?
	nullBit = nullAttributesIndicator[0] & (1 << 6);
	if (!nullBit) {
		memcpy((char *) buf + offset, &c, sizeof(float));
		offset += sizeof(float);
	}
	
}

int populateLeftTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);
	
	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("left", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);
	
	for (int i = 0; i < tupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		// a in [0,99], b in [10, 109], c in [50, 149.0]
		int a = i;
		int b = i + 10;
		float c = (float) (i + 50);
		prepareLeftTuple(attrs.size(), nullsIndicator, a, b, c, buf);

		rc = rm->insertTuple("left", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateLargeLeftTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);
	
	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("largeleft", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);
	
	for (int i = 0; i < largeTupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		// a in [0,49999], b in [10, 50009], c in [50, 50049.0]
		int a = i;
		int b = i + 10;
		float c = (float) (i + 50);
		prepareLeftTuple(attrs.size(), nullsIndicator, a, b, c, buf);

		rc = rm->insertTuple("largeleft", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateRightTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);

	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("right", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

	for (int i = 0; i < tupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		// b in [20, 119], c in [25, 124.0], d in [0, 99]
		int b = i + 20;
		float c = (float) (i + 25);
		int d = i;
		prepareRightTuple(attrs.size(), nullsIndicator, b, c, d, buf);

		rc = rm->insertTuple("right", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateLargeRightTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);

	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("largeright", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

	for (int i = 0; i < largeTupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		// b in [20, 50019], c in [25, 50024.0], d in [0, 49999]
		int b = i + 20;
		float c = (float) (i + 25);
		int d = i;
		prepareRightTuple(attrs.size(), nullsIndicator, b, c, d, buf);

		rc = rm->insertTuple("largeright", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateLeftVarCharTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);

	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("leftvarchar", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);

	for (int i = 0; i < varcharTupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		int a = i + 20;

		int length = (i % 26) + 1;
		string b = string(length, '\0');
		for (int j = 0; j < length; j++) {
			b[j] = 96 + length;
		}
		prepareLeftVarCharTuple(attrs.size(), nullsIndicator, a, length, b, buf);

		rc = rm->insertTuple("leftvarchar", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateRightVarCharTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);

	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("rightvarchar", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);
	
	for (int i = 0; i < varcharTupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		int length = (i % 26) + 1;
		string b = string(length, '\0');
		for (int j = 0; j < length; j++) {
			b[j] = 96 + length;
		}

		float c = (float) (i + 10);
		prepareRightVarCharTuple(attrs.size(), nullsIndicator, length, b, c, buf);

		rc = rm->insertTuple("rightvarchar", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int populateGroupTable() {
	// Functions Tested
	// 1. InsertTuple
	RC rc = success;
	RID rid;
	void *buf = malloc(bufSize);
	
	// GetAttributes
    vector<Attribute> attrs;
    rc = rm->getAttributes("group", attrs);
    assert(rc == success && "RelationManager::getAttributes() should not fail.");

    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicator(attrs.size());
    unsigned char *nullsIndicator = (unsigned char *) malloc(nullAttributesIndicatorActualSize);
	memset(nullsIndicator, 0, nullAttributesIndicatorActualSize);
	
	for (int i = 0; i < tupleCount; ++i) {
		memset(buf, 0, bufSize);

		// Prepare the tuple data for insertion
		// a in repetition of [1,5], b in repetition of [1, 5], c in [50, 149.0]
		int a = i%5 + 1;
		int b = i%5 + 1;
		float c = (float) (i + 50);
		prepareLeftTuple(attrs.size(), nullsIndicator, a, b, c, buf);

		rc = rm->insertTuple("group", buf, rid);
		if (rc != success) {
			goto clean_up;
		}
	}

clean_up:
	free(buf);
	return rc;
}

int createIndexforLeftB() {
	return rm->createIndex("left", "B");
}

int createIndexforLeftC() {
	return rm->createIndex("left", "C");
}

int createIndexforRightB() {
	return rm->createIndex("right", "B");
}

int createIndexforRightC() {
	return rm->createIndex("right", "C");
}

int deleteAndCreateCatalog() {
  // Try to delete the System Catalog.
  // If this is the first time, it will generate an error. It's OK and we will ignore that.
  RC rc = rm->deleteCatalog();

  rc = rm->createCatalog();
  assert (rc == success && "Creating the Catalog should not fail.");
  
  return rc;
}

#endif


