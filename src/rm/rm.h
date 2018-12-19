
#ifndef _rm_h_
#define _rm_h_

#include <string>
#include <vector>
#include <functional>
#include <math.h>
#include <climits>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>

#include "../rbf/rbfm.h"
#include "../ix/ix.h"


using namespace std;

typedef vector<RID> RIDs;
typedef map<int, RIDs> TableRids;

# define RM_EOF (-1)  // end of a scan operator

// RM_ScanIterator is an iteratr to go through tuples
class RM_ScanIterator {

public:
  RM_ScanIterator() {};
  ~RM_ScanIterator() {};
  RC getNextTuple(RID &rid, void *data);
  RC close();
  RBFM_ScanIterator rbfmsi;
};

class RM_IndexScanIterator {
 public:
  RM_IndexScanIterator(){};  	// Constructor
  ~RM_IndexScanIterator(){}; 	// Destructor

   RBFM_ScanIterator rbfm_scanIterator;
   IX_ScanIterator ix_scanIterator;
  // "key" follows the same format as in IndexManager::insertEntry()
  RC getNextEntry(RID &rid, void *key){return rbfm_scanIterator.getNextRecord(rid,key);};  	// Get next matching entry
  RC close(){rbfm_scanIterator.close();return 0;};             			// Terminate index scan
};


// Relation Manager
class RelationManager
{
public:
  static RelationManager* instance();

  RC createCatalog();

  RC deleteCatalog();

  RC createTable(const string &tableName, const vector<Attribute> &attrs);

  RC prepColumnsTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int table_id, const int column_name_length, const string &column_name, const int column_type, const int column_length, const int column_position, void *buffer, int *tupleSize);

  RC prepTablesTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int table_id, const int name_length, const string &table_name, void *buffer, int *tupleSize);

  RC deleteTable(const string &tableName);

  RC getAttributes(const string &tableName, vector<Attribute> &attrs);

  RC insertTuple(const string &tableName, const void *data, RID &rid);

  RC deleteTuple(const string &tableName, const RID &rid);

  RC updateTuple(const string &tableName, const void *data, const RID &rid);

  RC readTuple(const string &tableName, const RID &rid, void *data);

  // mainly for debugging
  // Print a tuple that is passed to this utility method.
  RC printTuple(const vector<Attribute> &attrs, const void *data);

  // mainly for debugging
  RC readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data);

  // scan returns an iterator to allow the caller to go through the results one by one.
  RC scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  // comparison type such as "<" and "="
      const void *value,                    // used in the comparison
      const vector<string> &attributeNames, // a list of projected attributes
      RM_ScanIterator &rm_ScanIterator);


  RC createIndex(const string &tableName, const string &attributeName);

  RC destroyIndex(const string &tableName, const string &attributeName);

    // indexScan returns an iterator to allow the caller to go through qualified entries in index
  RC indexScan(const string &tableName,
                        const string &attributeName,
                        const void *lowKey,
                        const void *highKey,
                        bool lowKeyInclusive,
                        bool highKeyInclusive,
                        RM_IndexScanIterator &rm_IndexScanIterator);

// Extra credit work (10 points)
public:
  RC dropAttribute(const string &tableName, const string &attributeName);

  RC addAttribute(const string &tableName, const Attribute &attr);


protected:
  RelationManager();
  ~RelationManager();

private:
  static RelationManager *_rm;
  vector<Attribute> columnsDescriptor;
  vector<Attribute> tablesDescriptor;
  hash<string> hash_function;
  RecordBasedFileManager *rbfm=RecordBasedFileManager::instance();
  TableRids columnRids;

};

#endif
