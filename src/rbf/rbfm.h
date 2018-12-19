#ifndef _rbfm_h_
#define _rbfm_h_

#include <string>
#include <vector>
#include <climits>

#include "../rbf/pfm.h"

using namespace std;

typedef struct
{
  unsigned pageNum;
  unsigned slotNum;
} RID;

typedef enum { TypeInt = 0, TypeReal, TypeVarChar } AttrType;

typedef unsigned AttrLength;

struct Attribute {
    string   name;     // attribute name
    AttrType type;     // attribute type
    AttrLength length; // attribute length
};

typedef enum { NO_OP = 0,  // no condition
		   EQ_OP,      // =
           LT_OP,      // <
           GT_OP,      // >
           LE_OP,      // <=
           GE_OP,      // >=
           NE_OP,      // !=
} CompOp;



/****************************************************************************
The scan iterator is NOT required to be implemented for part 1 of the project
*****************************************************************************/

# define RBFM_EOF (-1)

class RecordBasedFileManager;

class RBFM_ScanIterator {
public:
  RBFM_ScanIterator();
  ~RBFM_ScanIterator() {};
  RC getNextRecord(RID &rid, void *data);
  RC close() ;

  FileHandle fileHandle;
  vector<Attribute> recordDescriptor;
  vector<string> attributeNames;
  void *value;
  RID curr_rid;
  CompOp compOp;
  string conditionAttribute;
  RecordBasedFileManager *rbfm;
};


class RecordBasedFileManager
{
public:
  static RecordBasedFileManager* instance();

  RC createFile(const string &fileName);

  RC destroyFile(const string &fileName);

  RC openFile(const string &fileName, FileHandle &fileHandle);

  RC closeFile(FileHandle &fileHandle);

  RC create_header(FileHandle &fileHandle, int header_num);
  int get_header_page_num(int pageNum);
  int directory_page(int pageNum);
  int data_page(int pageNum);
  int free_space_index(int pageNum);
	RC new_page(FileHandle &fileHandle);

	//RC available_page(FileHandle &fileHandle, int space, RID &rid);

	RC available_page(FileHandle &fileHandle, int space, RID &rid);
RC setBitMap(RID &rid, FileHandle &fileHandle);
	RC UnsetBitMap(const RID &rid, FileHandle &fileHandle);

  RC insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, RID &rid);

  RC readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data);

  // This method will be mainly used for debugging/testing
  RC printRecord(const vector<Attribute> &recordDescriptor, const void *data);
  int calculate_header_page_num(int pageNum);
  RC overflow_occured(FileHandle &fileHandle, int header_page_num);

/**************************************************************************************************************************************************************
IMPORTANT, PLEASE READ: All methods below this comment (other than the constructor and destructor) are NOT required to be implemented for part 1 of the project
***************************************************************************************************************************************************************/
  RC deleteRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid);
   RC deleteRecords(FileHandle &fileHandle);

  RC updateRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const void *data, const RID &rid);

  RC readAttribute(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, const string &attributeName,
  void *data);
  RC reorganizePage(FileHandle &fileHandle,
    const vector<Attribute> &recordDescriptor, const unsigned pageNumber);
  RC scan(FileHandle &fileHandle,
      const vector<Attribute> &recordDescriptor,
      const string &conditionAttribute,
      const CompOp compOp,
      const void *value,
      const vector<string> &attributeNames,
      RBFM_ScanIterator &rbfm_ScanIterator);

// RC RecordBasedFileManager::deleteRecords(FileHandle &fileHandle);

protected:
  RecordBasedFileManager();
  ~RecordBasedFileManager();

private:
  static RecordBasedFileManager *_rbf_manager;
  PagedFileManager *pfm=PagedFileManager::instance();
};




#endif

