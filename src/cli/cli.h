#ifndef _cli_h_
#define _cli_h_

#include <string>
#include <cstring>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <unordered_map>

#include "../shared.h"
#include "../rbf/pfm.h"
#include "../rbf/rbfm.h"
#include "../rm/rm.h"
#include "../ix/ix.h"
#include "../qe/qe.h"

using namespace std;

typedef enum{ FILTER = 0, PROJECT, BNL_JOIN, INL_JOIN, GH_JOIN, AGG, IDX_SCAN, TBL_SCAN } QUERY_OP;

// Return code
typedef int RC;

struct Table {
  string tableName;
  vector<Attribute> columns;

};

// Record Manager
class CLI
{
public:
  static CLI* Instance();

  RC process(const string input);
  RC start();

protected:
  CLI();
  ~CLI();

private:
  // cli parsers
  RC createTable();
  RC createIndex();
  RC createCatalog();
  RC dropTable();
  RC dropIndex(const string tableName="", const string columnName="", bool fromCommand=true);
  RC dropCatalog();
  RC addAttribute();
  RC insertTuple();
  RC dropAttribute();
  RC load();
  RC printTable(const string tableName);
  RC printAttributes();
  RC printIndex();
  RC help(const string input);
  RC history();

  // query parsers
  // code [0,4]: operation number
  // code -1: operation not found
  // code -2: don't call isIterator
  Iterator * query(Iterator *previous, int code=-1);
  Iterator * createBaseScanner(const string token);
  Iterator * projection(Iterator *input);
  Iterator * filter(Iterator *input);
  Iterator * blocknestedloopjoin(Iterator *input);
  Iterator * indexnestedloopjoin(Iterator *input);
  Iterator * gracehashjoin(Iterator *input);
  Iterator * aggregate(Iterator *input);

  // run the query
  RC run(Iterator *);

  RC createProjectAttributes(const string tableName, vector<Attribute> &attrs);
  RC createCondition(const string tableName, Condition &condition, const bool join=false, const string joinTable="");
  RC createAttribute(Iterator *, Attribute &attr);
  RC createAggregateOp(const string operation, AggregateOp &op);

  void addTableNameToAttrs(const string tableName, vector<string> &attrs);
  bool isIterator(const string token, int &code);
  string getTableName(Iterator *it);
  string getAttribute(const string input);
  string fullyQualify(const string attribute, const string tableName);

  // cli catalog functions
  RC getAttributesFromCatalog(const string tableName, vector<Attribute> &columns);
  RC getAttribute(const string tableName, const string attrName, Attribute &attr);
  RC addAttributeToCatalog(const Attribute &attr, const string tableName, const int position);
  RC addTableToCatalog(const string tableName, const string file_url, const string type);
  RC addIndexToCatalog(const string tableName, const string indexName);

  // helper functions
  char * next();
  bool expect(const string token, const string expected);
  bool expect(const char * token, const string expected);
  string toLower(string input);
  bool checkAttribute(const string tableName, const string columnName, RID &rid, bool searchColumns=true);
  RC error(const string errorMessage);
  RC error(uint errorCode);
  RC printOutputBuffer(vector<string> &buffer, uint mod);
  RC updateOutputBuffer(vector<string> &buffer, void *data, vector<Attribute> &attrs);
  RC insertTupleToDB(const string tableName, const vector<Attribute> attributes, const void *data, unordered_map<int, void *> indexMap);
  RC getAttribute(const string name, const vector<Attribute> pool, Attribute &attr);

  RelationManager * rm;
  static CLI * _cli;
};

#endif
