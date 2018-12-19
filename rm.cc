
#include<iostream>
#include "rm.h"
#include<stdlib.h>
#include<string.h>
vector<Attribute> attributes;
using namespace std;

RelationManager* RelationManager::_rm = 0;

RelationManager* RelationManager::instance()
{
    if(!_rm)
        _rm = new RelationManager();

    return _rm;
}

RelationManager::RelationManager()
{
	RecordBasedFileManager *rbfm = RecordBasedFileManager::instance();

}

RelationManager::~RelationManager()
{
}
////Reference: rmtest_extra_1.cpp
void RelationManager::catalogTable(vector<Attribute> &attributes)
{
	Attribute attr;
		attr.name = "Name";
		attr.type = TypeVarChar;
		attr.length = (AttrLength)50;
		attributes.push_back(attr);

		attr.name = "Type";
		attr.type = TypeInt;
		attr.length = (AttrLength)4;
		attributes.push_back(attr);

		attr.name = "Num_of_col";
		attr.type = TypeInt;
		attr.length = (AttrLength)4;
		attributes.push_back(attr);
}

void RelationManager::ColumnTable(vector<Attribute> &attributes)
{
	Attribute attr;
	attr.name = "Name_Table";
	attr.type = TypeVarChar;
	attr.length = (AttrLength)50;
	attributes.push_back(attr);

	attr.name = "Feature_Name";
	attr.type = TypeVarChar;
	attr.length = (AttrLength)30;
	attributes.push_back(attr);

	attr.name = "Type";
	attr.type = TypeInt;
	attr.length = (AttrLength)4;
	attributes.push_back(attr);

	attr.name = "Size";
	attr.type = TypeInt;
	attr.length = (AttrLength)4;
	attributes.push_back(attr);

	attr.name = "Position";
	attr.type = TypeInt;
	attr.length = (AttrLength)4;
	attributes.push_back(attr);
}

int record_Length(const void *data,int start, const vector <Attribute> &recordDescriptor,int size)
{
unsigned end=0;

for(int i=0;i<recordDescriptor.size() && i<size;i++)

	{
	if(recordDescriptor[i].type==TypeInt)
						{
							end+=4;
						}
					else if(recordDescriptor[i].type==TypeReal)
						{
							end+=4;
						}
					else if(recordDescriptor[i].type==TypeVarChar)
						{
							end+=*(int *)((char *)data+start+end)+4;
						}
	}
		return end;
}


void RelationManager::PrepareCatRecord( const string &tableName, int NameLength, void *buffer,int num)
{
    int offset = 0;
    memcpy((char *)buffer + offset, &NameLength, 4);
    memcpy((char *)buffer + offset, tableName.c_str(), NameLength);
    memcpy((char *)buffer + offset, &num, 4);
    offset += 8+NameLength;
}

RC RelationManager::createTable(const string &tableName, const vector<Attribute> &attrs)
{

	ColumnTable(attributes);

	RecordBasedFileManager *rbfm=RecordBasedFileManager::instance();
	FileHandle fileHandle;

	void* buffer=malloc(PAGE_SIZE);
	rbfm->createFile("tableName");
	rbfm->openFile("tableName",fileHandle);


    return -1;
}

RC RelationManager::deleteTable(const string &tableName)
{
    return -1;
}

RC RelationManager::getAttributes(const string &tableName, vector<Attribute> &attrs)
{
    return -1;
}

RC RelationManager::insertTuple(const string &tableName, const void *data, RID &rid)
{
    return -1;
}

RC RelationManager::deleteTuples(const string &tableName)
{
    return -1;
}

RC RelationManager::deleteTuple(const string &tableName, const RID &rid)
{
    return -1;
}

RC RelationManager::updateTuple(const string &tableName, const void *data, const RID &rid)
{
    return -1;
}

RC RelationManager::readTuple(const string &tableName, const RID &rid, void *data)
{
    return -1;
}

RC RelationManager::readAttribute(const string &tableName, const RID &rid, const string &attributeName, void *data)
{
    return -1;
}

RC RelationManager::reorganizePage(const string &tableName, const unsigned pageNumber)
{
    return -1;
}

RC RelationManager::scan(const string &tableName,
      const string &conditionAttribute,
      const CompOp compOp,                  
      const void *value,                    
      const vector<string> &attributeNames,
      RM_ScanIterator &rm_ScanIterator)
{
    return -1;
}

// Extra credit
RC RelationManager::dropAttribute(const string &tableName, const string &attributeName)
{
    return -1;
}

// Extra credit
RC RelationManager::addAttribute(const string &tableName, const Attribute &attr)
{
    return -1;
}

// Extra credit
RC RelationManager::reorganizeTable(const string &tableName)
{
    return -1;
}
