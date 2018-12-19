#include "rm.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <vector>
#include "../rbf/rbfm.h"
#include "rm.h"
#include "../ix/ix.h"


int getActualByteForNullsIndicatorRM(int fieldCount) {

    return ceil((double) fieldCount / CHAR_BIT);
}


RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager()  {


    Attribute attr;
    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    tablesDescriptor.push_back(attr);

    attr.name = "table-name";
    attr.type = TypeVarChar;
    attr.length = (AttrLength)50;
    tablesDescriptor.push_back(attr);

    attr.name = "file-name";
    attr.type = TypeVarChar;
    attr.length = (AttrLength)50;
    tablesDescriptor.push_back(attr);

    attr.name = "table-id";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    columnsDescriptor.push_back(attr);

    attr.name = "column-name";
    attr.type = TypeVarChar;
    attr.length = (AttrLength)50;
    columnsDescriptor.push_back(attr);

    attr.name = "column-type";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    columnsDescriptor.push_back(attr);

    attr.name = "column-length";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    columnsDescriptor.push_back(attr);

    attr.name = "column-position";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    columnsDescriptor.push_back(attr);
}

RelationManager::~RelationManager() {

}


RC RelationManager::createCatalog()
{
    RC rc = 0;
    int trc = rbfm->createFile("Tables");
    int crc = rbfm->createFile("Columns");
    if(trc == -1 || crc == -1) {
        cout << "Creating the catalog files failed" << endl;
        rc = -1;
    } else {

        rc = createTable("Tables", tablesDescriptor);
        if(rc==-1) {
            cout << "Creating Tables catalog failed" << endl;
        } else {


            rc = createTable("Columns",columnsDescriptor);
            if(rc == -1) {
                cout << "Creating Columns catalog failed" << endl;
            }
        }
    }
    return rc;
}

RC RelationManager::deleteCatalog()
{
    RC returnVal = 0;
    RC rc = 0;
    returnVal = rbfm->destroyFile("Tables");
    if(returnVal == -1) {
        rc = -1;
    }
    returnVal = rbfm->destroyFile("Columns");
    if(returnVal == -1) {
        rc = -1;
    }
    return returnVal;
}

RC RelationManager::prepTablesTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int table_id, const int name_length, const string &table_name, void *buffer, int *tupleSize) {
    int offset = 0;
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicatorRM(attributeCount);

    memcpy((char *)buffer + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
    offset += nullAttributesIndicatorActualSize;

    nullBit = nullAttributesIndicator[0] & (1 << 7);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &table_id, sizeof(int));
        offset += sizeof(int);
    }

    nullBit = nullAttributesIndicator[0] & (1 << 6);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &name_length, sizeof(int));
        offset += sizeof(int);
        memcpy((char *)buffer + offset, table_name.c_str(), name_length);
        offset += name_length;
    }

    nullBit = nullAttributesIndicator[0] & (1 << 5);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &name_length, sizeof(int));
        offset += sizeof(int);
        memcpy((char *)buffer + offset, table_name.c_str(), name_length);
        offset += name_length;
    }

    *tupleSize = offset;
    return 0;
}

RC RelationManager::prepColumnsTuple(int attributeCount, unsigned char *nullAttributesIndicator, const int table_id, const int column_name_length, const string &column_name, const int column_type, const int column_length, const int column_position, void *buffer, int *tupleSize) {

    int offset = 0;
    bool nullBit = false;
    int nullAttributesIndicatorActualSize = getActualByteForNullsIndicatorRM(attributeCount);

    memcpy((char *)buffer + offset, nullAttributesIndicator, nullAttributesIndicatorActualSize);
    offset += nullAttributesIndicatorActualSize;

    nullBit = nullAttributesIndicator[0] & (1 << 7);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &table_id, sizeof(int));
        offset += sizeof(int);
    }

    // Is the age field not-NULL?
    nullBit = nullAttributesIndicator[0] & (1 << 6);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &column_name_length, sizeof(int));
        offset += sizeof(int);
        memcpy((char *)buffer + offset, column_name.c_str(), column_name_length);
        offset += column_name_length;
    }
    nullBit = nullAttributesIndicator[0] & (1 << 5);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &column_type, sizeof(int));
        offset += sizeof(int);
    }

    nullBit = nullAttributesIndicator[0] & (1 << 4);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &column_length, sizeof(int));
        offset += sizeof(int);
    }

    nullBit = nullAttributesIndicator[0] & (1 << 3);

    if (!nullBit) {
        memcpy((char *)buffer + offset, &column_position, sizeof(int));
        offset += sizeof(int);
    }

    *tupleSize = offset;
    return 0;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{
    RC rc = 0;
    FileHandle fileHandle;
    if(tableName!="Tables" && tableName!="Columns") {
        rc = rbfm->createFile(tableName);
        if(rc==-1) {
            cout << "Createfile failed in createTable" << endl;
        }
    }
    RID rid;
    RC prep_rc, insert_rc;
    int tupleSize = 0;
    void *tuple = malloc(200);
    int nullAttributesIndicatorActualSizeTable = getActualByteForNullsIndicatorRM(tablesDescriptor.size());
    unsigned char *nullsIndicatorTable = (unsigned char *) malloc(nullAttributesIndicatorActualSizeTable);
    memset(nullsIndicatorTable, 0, nullAttributesIndicatorActualSizeTable);
    int table_id = (int)hash_function(tableName);
    columnRids[table_id] = RIDs();
    prep_rc = prepTablesTuple(tablesDescriptor.size(), nullsIndicatorTable, table_id, tableName.length(), tableName, tuple, &tupleSize);
    insert_rc = insertTuple("Tables", tuple, rid);
    if(prep_rc==-1 || insert_rc==-1){
        cout << "Problem prepping and inserting a table record!" << endl;
        rc = -1;
    } else {
        int nullAttributesIndicatorActualSizeColumn = getActualByteForNullsIndicatorRM(columnsDescriptor.size());
        unsigned char *nullsIndicatorColumn = (unsigned char *) malloc(nullAttributesIndicatorActualSizeColumn);
        memset(nullsIndicatorColumn, 0, nullAttributesIndicatorActualSizeColumn);
        for(unsigned i = 0; i < attrs.size(); i++) {
            RID rid;
            tupleSize = 0;
            prep_rc = prepColumnsTuple(tablesDescriptor.size(), nullsIndicatorColumn, table_id, attrs[i].name.length(), attrs[i].name, attrs[i].type, attrs[i].length, i, tuple, &tupleSize);
            insert_rc = insertTuple("Columns", tuple, rid);
            columnRids[table_id].push_back(rid);
            if(prep_rc==-1 || insert_rc==-1){
                cout << "Problem prepping and inserting a column record!" << endl;
                rc = -1;
                break;
            }
        }
    }

    // rc = rbfm->openFile("Columns",fileHandle);
    // rbfm->closeFile(fileHandle);
    // rc = rbfm->openFile("Tables",fileHandle);
    // rbfm->closeFile(fileHandle);
    free(tuple);
    tuple = NULL;
    return rc;
}

RC RelationManager::deleteTable(const string &tableName)
{
    RC rc;
    rc = rbfm->destroyFile(tableName);
    /*Still need to remove records from catalog*/
    return rc;
}

RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
    /*
    Get attributes should return all columns (loaded into an attribute vector) from the columns table
    that match the table-id (hash tableName)
    */

    RC rc = 0;
    FileHandle fileHandle;
    Attribute attr;
    void *returnedData = malloc(PAGE_SIZE);
    rc = rbfm->openFile("Columns",fileHandle);
    if(rc == -1) {
        cout << "Opening Columns catalog failed in getAttributes" << endl;
    } else {
        int table_id = (int)hash_function(tableName);
        int fields=columnsDescriptor.size();
        int null_bytes=ceil(double(fields)/8);
        int name_length;
        int len;

        RBFM_ScanIterator rbfmsi;
        vector<string> attributes;
        string name = "column-name";
        attributes.push_back(name);
        string type = "column-type";
        attributes.push_back(type);
        string length = "column-length";
        attributes.push_back(length);
        int *value = (int *) malloc(4);
        *value=table_id;

        void *returnedData = malloc(PAGE_SIZE);
        RID rid;
        rbfm->scan(fileHandle, columnsDescriptor, "table-id", EQ_OP, value, attributes, rbfmsi);

        int numNullBytes=ceil(double(attributes.size())/8);
        unsigned char *nullbytes = (unsigned char *) malloc(numNullBytes);

        int pointer = numNullBytes;
        char * nameString;

        while(rbfmsi.getNextRecord(rid, returnedData) != RBFM_EOF) {
            pointer = numNullBytes;
            name_length = *(int *)((char *)returnedData+pointer);
            pointer = pointer + 4;
            nameString = new char[name_length+1];
            for(int j=0;j<name_length;j++) {
                nameString[j] = *((char *)returnedData+pointer+j);
            }
            nameString[name_length] = '\0';
            pointer+=name_length;
            attr.name.assign(nameString);
            delete[] nameString;
            len = *(int*)((char *)returnedData+pointer);
            attr.type = static_cast<AttrType>(len);
            pointer+=4;
            attr.length = *(int*)((char *)returnedData+pointer);
            attrs.push_back(attr);
        }
    }
    RC rcd = rbfm->closeFile(fileHandle);
    return 0;
}

RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{
    /*
    Insert a record into the table tableName with the given recordID, no need to check attributes
    Using the RBF call insertRecord
    The only thing needed here is the recordDescriptor, which we can build from getAttributes
    Assumption is data is formatted properly, tests use prepareTuple and like functions
    */
    RC returnVal = 0;
    FileHandle fileHandle;
    returnVal = rbfm->openFile(tableName,fileHandle);
    if(returnVal==-1){
        cout << "We did not open the file correctly in insertTuple" << endl;
        cout << "Tablename: " << tableName << endl;
    } else {

        if(tableName == "Tables") {
            returnVal = rbfm->insertRecord(fileHandle, tablesDescriptor, data, rid);
        }
        else if (tableName == "Columns") {
            returnVal = rbfm->insertRecord(fileHandle, columnsDescriptor, data, rid);
        }
        else {
            vector<Attribute> recordDescriptor;
            getAttributes(tableName,recordDescriptor);
            returnVal = rbfm->insertRecord(fileHandle, recordDescriptor, data, rid);
        }
    }
    RC returnVald = rbfm->closeFile(fileHandle);
    return returnVal;
}

RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
    /*
    Remove a record from tableName with the given recordID with an RBF call to deleteRecord
    */
    RC returnVal;
    FileHandle fileHandle;
    returnVal = rbfm->openFile(tableName,fileHandle);
    if(returnVal==-1){
        cout << "We did not open the file correctly in deleteTuple" << endl;
    } else {
        vector<Attribute> recordDescriptor;
        getAttributes(tableName,recordDescriptor);
        returnVal = rbfm->deleteRecord(fileHandle, recordDescriptor, rid);
    }
    RC returnVald = rbfm->closeFile(fileHandle);
    return returnVal;
}

RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
    /*
    Update tuple, should call RBF updateRecord with the given RID and data
    */
    RC rc;
    FileHandle fileHandle;
    rc = rbfm->openFile(tableName,fileHandle);
    vector<Attribute> recordDescriptor;
    rc = getAttributes(tableName,recordDescriptor);
    if(rc != -1) {
        rc = rbfm->updateRecord(fileHandle, recordDescriptor, data, rid);
    }
    RC rcd = rbfm->closeFile(fileHandle);
    return rc;
}

RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
    /*
    readTuple should read a given record into data, using readRecord
    */
    RC rc;
    FileHandle fileHandle;
    rc = rbfm->openFile(tableName,fileHandle);
    if(rc!=-1) {
        vector<Attribute> tupleDescriptor;
        getAttributes(tableName,tupleDescriptor);
        rc = rbfm->readRecord(fileHandle, tupleDescriptor, rid, data);
    }
    RC rcd = rbfm->closeFile(fileHandle);
    return rc;
}

RC RelationManager::printTuple(const vector<Attribute> &attrs, const void *data)
{
    /*
    print tuple calls print record...
    */
    RC rc;
    rc = rbfm->printRecord(attrs, data);
    return 0;
}

RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{
    /*
    Use readAttribute RBF call
    */
    RC rc;
    FileHandle fileHandle;
    rc = rbfm->openFile(tableName,fileHandle);
    vector<Attribute> tupleDescriptor;
    rc = getAttributes(tableName,tupleDescriptor);
    if(rc != -1) {
        rc = rbfm->readAttribute(fileHandle, tupleDescriptor, rid, attributeName, data);
    }
    RC rcd=rbfm->closeFile(fileHandle);
    return rc;
}

RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,
      const void *value,
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{
    RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();
    FileHandle fileHandle;
    RC returnval = rbfm->openFile(tableName, fileHandle);
    vector<Attribute> recordDescriptor;
    returnval = getAttributes(tableName, recordDescriptor);
    returnval = rbfm->scan(fileHandle, recordDescriptor, conditionAttribute, compOp, value, attributeNames,rm_ScanIterator.rbfmsi);
    return returnval;
}

// "data" follows the same format as RelationManager::insertTuple()
RC RM_ScanIterator::getNextTuple(RID &rid, void *data) {
    
    return 0;
};

RC RM_ScanIterator::close() {
    RC returnVal = rbfmsi.close();
    return returnVal;
};


//********************************* QE related ---- need to check initializations ------*******************


RC RelationManager::createIndex(const string &tableName, const string &attributeName)
{
	void *buffer=malloc(PAGE_SIZE);
	RID rid;
	IndexManager *ix=IndexManager::instance();
	RC success=ix->createFile("index_"+tableName+"_"+attributeName);

	FileHandle fileHandle;
	IXFileHandle ixfileHandle;

	RecordBasedFileManager *rbfm=RecordBasedFileManager::instance();

	success=success|rbfm->openFile(tableName,fileHandle);
	success=success|ix->openFile("index_"+tableName+"_"+attributeName,ixfileHandle);

	vector <string> x;
	x.push_back(attributeName);
	vector <Attribute> recordDescriptor;
	getAttributes(tableName,recordDescriptor);
	RBFM_ScanIterator rbfm_scanIterator;
	success=success|rbfm->scan(fileHandle,recordDescriptor,"",NO_OP,NULL,x,rbfm_scanIterator);

	int i;
	for(int i=0;i<recordDescriptor.size();i++)
		if(recordDescriptor[i].name==attributeName)
			break;
	while(rbfm_scanIterator.getNextRecord(rid,buffer)!=-1)
	{
		success=success|ix->insertEntry(ixfileHandle,recordDescriptor[i],buffer,rid);
	}
	success=success|ix->closeFile(ixfileHandle);
	success=success|rbfm->closeFile(fileHandle);
	
	free(buffer);
	buffer=NULL;

	return success;

}

RC RelationManager::destroyIndex(const string &tableName, const string &attributeName)
{
	IndexManager *ix=IndexManager::instance();
	RC success = ix->destroyFile("index_"+tableName+"_"+attributeName);
	return success;

}

RC RelationManager::indexScan(const string &tableName,
                      const string &attributeName,
                      const void *lowKey,
                      const void *highKey,
                      bool lowKeyInclusive,
                      bool highKeyInclusive,
                      RM_IndexScanIterator &rm_IndexScanIterator)
{
	IndexManager *ix=IndexManager::instance();
	IXFileHandle ixfileHandle;
	int success=0;
	// open ix file
	success=success|ix->openFile("index_"+tableName+"_"+attributeName,ixfileHandle);
	IX_ScanIterator ix_scanIterator;

	vector <Attribute> recordDescriptor;
	getAttributes(tableName,recordDescriptor);

	int i=0;
	for(i=0;i<recordDescriptor.size();i++)
		if(recordDescriptor[i].name==attributeName)
			break;
	if(i==recordDescriptor.size())
		return -1;
	success=success|ix->scan(ixfileHandle,recordDescriptor[i],lowKey,highKey,lowKeyInclusive,highKeyInclusive,ix_scanIterator);
	rm_IndexScanIterator.ix_scanIterator=ix_scanIterator;

	return success;
}


// Extra credit work
RC RelationManager::addAttribute(const string &tableName, const Attribute &attr)
{
    return -1;
}

// Extra credit work
RC RelationManager::dropAttribute(const string &tableName, const string &attributeName)
{
    return -1;
}
