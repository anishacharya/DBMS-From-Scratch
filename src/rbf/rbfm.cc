#include <iostream>
#include "rbfm.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cmath>
#include "rbfm.h"

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager() {
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}
    RC RecordBasedFileManager::createFile(const string &fileName)
{
	RC result= pfm->createFile(fileName.c_str());

	// Create a HeaderPage when creating a File

	FileHandle fileHandle;
	pfm->openFile(fileName.c_str(), fileHandle);
	result=create_header(fileHandle,0);
	return result;
}
RC RecordBasedFileManager::destroyFile(const string &fileName)
{
		RC result= pfm->destroyFile(fileName.c_str());
		return result;
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle)
{
		RC result= pfm->openFile(fileName.c_str(),fileHandle);
		return result;
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle)
{
		RC result= pfm->closeFile(fileHandle);
		return result;
}

int record_length(const void *data,int start, const vector <Attribute> &recordDescriptor)
{

	int fields=recordDescriptor.size();
	int null_bytes=ceil(double(fields)/8);

	unsigned char *nullsIndicator = (unsigned char *) malloc(null_bytes);

	memcpy(nullsIndicator,(char *)data + start, null_bytes);
	unsigned end=null_bytes;

	int shifterbits = (null_bytes*8) - 1;
	bool nullBit = false;

	for(unsigned i=0;i<recordDescriptor.size();i++)
	{
		nullBit = nullsIndicator[0] & (1 << shifterbits);
		if(!nullBit) {
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
		shifterbits--;
	}
	return end;
}

RC RecordBasedFileManager::create_header(FileHandle &fileHandle,int header_num)
{
	int returnval =0;
	void * buffer=malloc(PAGE_SIZE);
	int offset=0;

	int *no_page = (int *) malloc(4);
	*no_page=9999;

	int *no_overflow = (int *) malloc(4);
	*no_overflow=0;

	for(int i =0; i<1023;i++) {
		memcpy((char *)buffer+offset,(char *)no_page, 4);
		offset=offset+4;
	}
	memcpy((char *)buffer+4092, (char *)no_overflow, 4);
	returnval = fileHandle.appendPage(buffer);
	free(buffer);
	buffer=NULL;
	return returnval;
}

RC RecordBasedFileManager::new_page(FileHandle &fileHandle)
{
	RC returnVal;
	int numPages = fileHandle.getNumberOfPages();
	void * directory_page=malloc(PAGE_SIZE);

	int *no_slot = (int *) malloc(4);
	*no_slot=9999;
	int offset=0;
	for(int i =0; i<1023;i++) {
		memcpy((char *)directory_page+offset,(char *)no_slot, 4);
		offset=offset+4;
	}

	returnVal = fileHandle.appendPage(directory_page);
	free(directory_page);
	directory_page=NULL;


	void * data_page=malloc(PAGE_SIZE);
	returnVal = fileHandle.appendPage(data_page);
	free(data_page);
	data_page=NULL;

	int header_page_num=((numPages/2047)*2047);
	offset=((numPages-header_page_num)-1)/2;

	void * buffer=malloc(PAGE_SIZE);
	fileHandle.readPage(header_page_num,buffer);

	int *free_space = (int *) malloc(4);
	*free_space=4096;

	memcpy((char *) buffer+(offset*4), (char*)free_space, 4);
	fileHandle.writePage(header_page_num,buffer);
	free(buffer);
	buffer=NULL;
	return 0;
}

RC RecordBasedFileManager::overflow_occured(FileHandle &fileHandle, int header_page_num) {

	void * buffer=malloc(PAGE_SIZE);
	int *overflow= (int *) malloc(4);
	*overflow=1;
	fileHandle.readPage(header_page_num,buffer);
	memcpy((char *)buffer+4092,(char *)overflow, 4);
	fileHandle.writePage(header_page_num,buffer);
	free(buffer);
	buffer=NULL;
}

RC RecordBasedFileManager::available_page(FileHandle &fileHandle, int space, RID &rid)
{
	RC returnVal = 0;
	unsigned openPage = 0;
	int header_page_num=0;
	void * header_buffer=malloc(PAGE_SIZE);
	bool searching=true;
	int offset=0;
	int i;
	while(searching) {
		fileHandle.readPage(header_page_num,header_buffer);
		offset=0;
		for(i=0;i < 1023; i++) {
			if(*(int *)((char *)header_buffer+offset) == 9999) {
				new_page(fileHandle);
				rid.pageNum=i+(header_page_num/2);
				searching=false;
				break;
			}
			if( *(int *)((char *)header_buffer+offset) >= space){
				rid.pageNum=i+(header_page_num/2);
				searching=false;
				break;
			}
			offset=offset+4;
		}
		if(searching){
			if(*(int *)((char *)header_buffer+(i*4))==0) {
				overflow_occured(fileHandle,header_page_num);
				create_header(fileHandle,header_page_num);
			}
			header_page_num=header_page_num+2047;
		}
	}
	free(header_buffer);
	header_buffer=NULL;
	return returnVal;
}

int RecordBasedFileManager::get_header_page_num(int pageNum)
{
	return ((pageNum/1023)*2047);   // This is wrong see 2045/1063=1 but 2045 shd be indexed by header 0
	//return ((pageNum/2047)*2047);
}

int RecordBasedFileManager::data_page(int pageNum) {
	int header_page_num = get_header_page_num(pageNum);
	int header_page_case = header_page_num%2;
	int directory_page_num;
	switch(header_page_case){
		case 0:
			directory_page_num=(pageNum*2)+2;
			break;
		case 1:
			directory_page_num=(pageNum*2)+3;
			break;
	}
	return directory_page_num;
}

int RecordBasedFileManager::directory_page(int pageNum) {
	int header_page_num = get_header_page_num(pageNum);
	int header_page_case = header_page_num%2;
	int data_page_num;
	switch(header_page_case){
		case 0:
			data_page_num=(pageNum*2)+1;
			break;
		case 1:
			data_page_num=(pageNum*2)+2;
			break;
	}
	return data_page_num;
}

int RecordBasedFileManager::free_space_index(int pageNum){
	int header_page_num = get_header_page_num(pageNum);
	return (pageNum-(header_page_num/2))*4;
}

RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor,const void *data, RID &rid)
{
	int record_size =record_length(data,0,recordDescriptor);

	int *space = (int *) malloc(4);
	*space=record_size;

	RC returnVal=available_page(fileHandle, record_size, rid);


	int header_page_num = get_header_page_num(rid.pageNum);

	void* data_buffer=malloc(PAGE_SIZE);
	void* directory_buffer=malloc(PAGE_SIZE);
	void* header_buffer=malloc(PAGE_SIZE);
	int data_page_num=data_page(rid.pageNum);
	int directory_page_num=directory_page(rid.pageNum);

	fileHandle.readPage(directory_page_num,directory_buffer);
	fileHandle.readPage(data_page_num,data_buffer);
	fileHandle.readPage(header_page_num,header_buffer);

	int index = free_space_index(rid.pageNum);
	int free_space = *(int*)((char*)header_buffer+index);
	int *record_offset = (int *) malloc(4);
	*record_offset=PAGE_SIZE-free_space;

	int offset=0;
	for(int slot=0;slot<512;slot++) {
		if( *(int *)((char *)directory_buffer+offset) == 9999) {
			memcpy((char *)directory_buffer+offset,(char *)space,4);
			memcpy((char *)directory_buffer+offset+4,(char *)record_offset,4);
			memcpy((char *)data_buffer+*record_offset,(char *)data,record_size);
			rid.slotNum=slot;
			break;
		}
		offset=offset+8;
	}

	fileHandle.writePage(directory_page_num,directory_buffer);
	fileHandle.writePage(data_page_num,data_buffer);

	int *remaining_space = (int *) malloc(4);
	*remaining_space=free_space-record_size;
	memcpy((char*)header_buffer+index,(char *) remaining_space,4);

	fileHandle.writePage(header_page_num,header_buffer);

	free(header_buffer);
	header_buffer = NULL;
	free(data_buffer);
	data_buffer = NULL;
	free(directory_buffer);
	directory_buffer = NULL;

	return 0;
}



RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data)
{
	int header_page_num = get_header_page_num(rid.pageNum);

	void* data_buffer=malloc(PAGE_SIZE);
	void* directory_buffer=malloc(PAGE_SIZE);
	void * header_buffer=malloc(PAGE_SIZE);
	int data_page_num=data_page(rid.pageNum);
	int directory_page_num=directory_page(rid.pageNum);

	fileHandle.readPage(header_page_num,header_buffer);
	fileHandle.readPage(directory_page_num,directory_buffer);
	fileHandle.readPage(data_page_num,data_buffer);

	RC returnVal=0;
	int index = free_space_index(rid.pageNum);
	if(*(int *)((char *)header_buffer+index)!=9999) {
		if(*(int *)((char *)directory_buffer+(rid.slotNum*8))!=9999) {
			int record_length = *(int *)((char *)directory_buffer+(rid.slotNum*8));
			int record_offset = *(int *)((char *)directory_buffer+(rid.slotNum*8)+4);
			memcpy((char *)data,(char *)data_buffer+record_offset,record_length);
			if(record_length==12) {
				int tombstone = *(int *)((char *)data);
				if(tombstone==6666) {
					RID new_rid;
					new_rid.pageNum = *(int *)((char *)data+4);
					new_rid.slotNum = *(int *)((char *)data+8);
					RC returnval = readRecord(fileHandle,recordDescriptor,new_rid,data);
					if(returnval==-1){
						returnVal=-1;
					}
				}
			}
		} else {
			returnVal = -1;
		}
	} else {
		returnVal=-1;
	}

	free(header_buffer);
	header_buffer = NULL;
	free(data_buffer);
	data_buffer = NULL;
	free(directory_buffer);
	directory_buffer = NULL;

	return returnVal;
}




RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data)
{
	int pointer =0;

	int fields=recordDescriptor.size();
	int null_bytes=ceil(double(fields)/8);

	for(int i=0;i<recordDescriptor.size();i++)
	{
		string name = recordDescriptor[i].name;
		switch(recordDescriptor[i].type)
		{
		case TypeInt:
				cout << name << ": ";
				cout<<*(int*)((char *)data+null_bytes+pointer) << endl;
				pointer+=4;
				break;

		case TypeReal:
				cout << name << ": ";
				cout<<*(float*)((char *)data+null_bytes+pointer) << endl;;
				pointer+=4;
				break;

		case TypeVarChar:
				cout << name << ": ";
				for(int j=0;j<*(int *)((char *)data+null_bytes+pointer);j++)
					cout<<*((char *)data+pointer+null_bytes+4+j);
				cout << endl;
				pointer+=*(int *)((char *)data+null_bytes+pointer)+4;
				break;
		}
	}
    return 0;
}


RC RecordBasedFileManager::readAttribute(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const RID &rid,
		const string &attributeName, void *data)
{
	void * buffer=malloc(PAGE_SIZE);
	readRecord(fileHandle, recordDescriptor, rid, buffer);
	RC readattr = 0;

	int fields=recordDescriptor.size();
	int null_bytes=ceil(double(fields)/8);

	unsigned char *nullsIndicator = (unsigned char *) malloc(null_bytes);
	memcpy(nullsIndicator,(char *)buffer, null_bytes);
	unsigned end=null_bytes;
	int shifterbits = (null_bytes*8) - 1;
	bool nullBit = false;
	int pointer=0;

	unsigned char *returnbyte = (unsigned char *) malloc(1);
	memset(returnbyte, 0, 1);
	memcpy((char *)data,returnbyte,1);

	for(unsigned i=0;i<recordDescriptor.size();i++)
	{
		nullBit = nullsIndicator[0] & (1 << shifterbits);

		if(recordDescriptor[i].type==TypeInt)
		{
			if(recordDescriptor[i].name==attributeName) {
				if(!nullBit) {
					memcpy((char *)data+1,(char *)buffer+null_bytes+pointer,4);
				} else {
					readattr = -1;
				}
			}
			pointer+=4;
		}
		else if(recordDescriptor[i].type==TypeReal)
		{
			if(recordDescriptor[i].name==attributeName) {
				if(!nullBit) {
					memcpy((char *)data+1,(char *)buffer+null_bytes+pointer,4);
				} else {
					readattr = -1;
				}
			}
			pointer+=4;
		}
		else if(recordDescriptor[i].type==TypeVarChar)
		{
			if(recordDescriptor[i].name==attributeName) {
				if(!nullBit) {
					memcpy((char *)data+1,(char *)buffer+null_bytes+pointer,4);
					memcpy((char *)data+5,(char *)buffer+null_bytes+pointer+4,*(int *)data);
				} else {
					readattr = -1;
				}
			}
			pointer += *(int *)((char *)buffer+null_bytes+pointer) + 4;
		}
		shifterbits--;
	}

	free(buffer);
	buffer=NULL;
	free(nullsIndicator);
	nullsIndicator=NULL;
	return readattr;
}



RC RecordBasedFileManager::deleteRecord(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const RID &rid)
{
	RC returnVal=0;
	void *header_buffer=malloc(PAGE_SIZE);
	void *directory_buffer=malloc(PAGE_SIZE);
	void *data_buffer=malloc(PAGE_SIZE);

	int header_page_num=get_header_page_num(rid.pageNum);
	int data_page_num=data_page(rid.pageNum);
	int directory_page_num=directory_page(rid.pageNum);

	fileHandle.readPage(directory_page_num,directory_buffer);
	fileHandle.readPage(data_page_num,data_buffer);
	fileHandle.readPage(header_page_num,header_buffer);

	int index = free_space_index(rid.pageNum);
	int free_space = *(int*)((char*)header_buffer+index);

	int record_length = *(int *)((char *)directory_buffer+(rid.slotNum*8));
	int record_offset = *(int *)((char *)directory_buffer+(rid.slotNum*8)+4);
	if(record_length == 9999 || record_offset == 9999) {
		returnVal=1;
	}
	if(record_length==12) {
		int tombstone = *(int *)((char *)data_buffer+record_offset);
		if(tombstone==6666) {
			RID new_rid;
			new_rid.pageNum = *(int *)((char *)data_buffer+4);
			new_rid.slotNum = *(int *)((char *)data_buffer+8);
			RC returnval = deleteRecord(fileHandle,recordDescriptor,new_rid);
		}
	}
	int *null_offset = (int *) malloc(4);
	int *null_length = (int *) malloc(4);
	*null_offset=9999;
	*null_length=9999;

	memcpy((char*)directory_buffer+(rid.slotNum*8),(char *)null_length,4);
	memcpy((char*)directory_buffer+(rid.slotNum*8)+4,(char *)null_offset,4);
	int slotNum=rid.slotNum+1;
	int next_record_length = *(int *)((char *)directory_buffer+(slotNum*8));
	int next_record_offset = *(int *)((char *)directory_buffer+(slotNum*8)+4);
	int * pointer = (int *)malloc(4);
	*pointer = record_offset;
	while(slotNum < 512) {
		if(next_record_length != 9999 && next_record_offset != 9999) {
			memcpy((char*)data_buffer+*pointer, (char*)data_buffer+next_record_offset,next_record_length);
			memcpy((char*)directory_buffer+(slotNum*8)+4,(char *)pointer,4);
			if(rid.pageNum==5) {
			}
			*pointer = *pointer + next_record_length;
		}
		slotNum++;
		next_record_length = *(int *)((char *)directory_buffer+(slotNum*8));
		next_record_offset = *(int *)((char *)directory_buffer+(slotNum*8)+4);
	}
	int *new_free_space=(int *)malloc(4);
	*new_free_space=free_space+record_length;
	memcpy((char*)header_buffer+index,(char*)new_free_space,4);
	fileHandle.writePage(directory_page_num,directory_buffer);
	fileHandle.writePage(data_page_num,data_buffer);

	free(null_offset);
	free(null_length);
	free(data_buffer);
	free(header_buffer);
	free(directory_buffer);
	return 0;

}

RC RecordBasedFileManager::updateRecord(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const void *data, const RID &rid)
{
	void *data_buffer = malloc(PAGE_SIZE);
	void *header_buffer=malloc(PAGE_SIZE);
	void *directory_buffer=malloc(PAGE_SIZE);

	int header_page_num=get_header_page_num(rid.pageNum);
	int data_page_num=data_page(rid.pageNum);
	int directory_page_num=directory_page(rid.pageNum);

	fileHandle.readPage(directory_page_num,directory_buffer);
	fileHandle.readPage(data_page_num,data_buffer);
	fileHandle.readPage(header_page_num,header_buffer);

	int index = free_space_index(rid.pageNum);
	int free_space = *(int*)((char*)header_buffer+index);

	int *new_length = (int *) malloc(4);
	int *tombstone = (int *) malloc(4);
	int *new_page = (int *) malloc(4);
	int *new_slot = (int *) malloc(4);
	int *new_free_space = (int *)malloc(4);
	*tombstone=6666;
	bool post_insert=false;

	int new_record_length =record_length(data,0,recordDescriptor);
	int curr_record_length = *(int *)((char *)directory_buffer+(rid.slotNum*8));
	int record_offset = *(int *)((char *)directory_buffer+(rid.slotNum*8)+4);

	if(curr_record_length==12) {
		int tombstone = *(int *)((char *)data_buffer+record_offset);
		if(tombstone==6666) {
			RID new_rid;
			new_rid.pageNum = *(int *)((char *)data_buffer+4);
			new_rid.slotNum = *(int *)((char *)data_buffer+8);
			RC returnval = updateRecord(fileHandle,recordDescriptor,data,new_rid);
			return returnval;
		}
	}
	int slotNum = rid.slotNum+1;
	int next_record_length;
	int next_record_offset;
	int *pointer= (int * )malloc(4);
	*pointer = record_offset;
	RID new_rid;

	if(new_record_length!=curr_record_length) {
		if(new_record_length-curr_record_length > free_space) {
			*pointer=record_offset+12;
		} else {
			*pointer=record_offset+new_record_length;
		}

		int init_pointer=*pointer;
		next_record_length = *(int *)((char *)directory_buffer+(slotNum*8));
		next_record_offset = *(int *)((char *)directory_buffer+(slotNum*8)+4);
		while(slotNum < 512) {
			if(next_record_length != 9999 && next_record_offset != 9999) {
				if(next_record_length != 9999 && next_record_offset != 9999) {
					memcpy((char*)directory_buffer+(slotNum*8)+4,(char *)pointer,4);
					*pointer=*pointer+next_record_length;
				}
			}
			slotNum++;
			next_record_length = *(int *)((char *)directory_buffer+(slotNum*8));
			next_record_offset = *(int *)((char *)directory_buffer+(slotNum*8)+4);
		}
		int block_size=*pointer-init_pointer;
		memcpy((char*)data_buffer+init_pointer,(char *)data_buffer+record_offset+curr_record_length,block_size);
		if(new_record_length-curr_record_length > free_space) {
			post_insert=true;
			*new_length=12;
			*new_free_space=free_space+curr_record_length-12;
		}
		else{
			memcpy((char*)data_buffer+record_offset,(char*)data,new_record_length);
			*new_free_space=free_space-(new_record_length-curr_record_length);
			*new_length = new_record_length;
		}
		memcpy((char *)directory_buffer+(rid.slotNum*8),(char*)new_length,4);
 	} else {
		memcpy((char *)data_buffer+record_offset,(char*)data,new_record_length);
		*new_free_space=free_space;
	}

	memcpy((char*)header_buffer+index,(char*)new_free_space,4);
	fileHandle.writePage(data_page_num,data_buffer);
	fileHandle.writePage(directory_page_num,directory_buffer);
	fileHandle.writePage(header_page_num,header_buffer);
	if(post_insert) {
		RC ret=insertRecord(fileHandle, recordDescriptor,data, new_rid);
		*new_page=new_rid.pageNum;
		*new_slot=new_rid.slotNum;
		memcpy((char *)data_buffer+record_offset,(char*)tombstone,4);
		memcpy((char *)data_buffer+record_offset+4,(char*)new_page,4);
		memcpy((char *)data_buffer+record_offset+8,(char*)new_slot,4);
	}
	fileHandle.writePage(data_page_num,data_buffer);

	directory_page_num=directory_page(5);
	fileHandle.readPage(directory_page_num,directory_buffer);
	int test_slot =  *(int *)((char *)directory_buffer+(6*8)+4);

	free(new_length);
	free(tombstone);
	free(new_page);
	free(new_slot);
	free(new_free_space);
	free(data_buffer);
	free(directory_buffer);
	free(header_buffer);
	free(pointer);
	return 0;
}

RC RecordBasedFileManager::scan(FileHandle &fileHandle,
								const vector<Attribute> &recordDescriptor, const string &conditionAttribute,
      							const CompOp compOp, const void *value,
								const vector<string> &attributeNames,
								RBFM_ScanIterator &rbfm_ScanIterator)

{
	RC success=0;
	rbfm_ScanIterator.recordDescriptor=recordDescriptor;
	rbfm_ScanIterator.attributeNames=attributeNames;
	rbfm_ScanIterator.conditionAttribute=conditionAttribute;
	rbfm_ScanIterator.value = (void *)value;
	rbfm_ScanIterator.curr_rid.pageNum = 0;
	rbfm_ScanIterator.curr_rid.slotNum = 0;
	rbfm_ScanIterator.compOp=compOp;
	rbfm_ScanIterator.fileHandle=fileHandle;
    return success;
}

template <class Attr>
bool compare(Attr x1,Attr x2,CompOp compOp)
{
	if(compOp==EQ_OP)
		return x1==x2;
	if(compOp==LT_OP)
		return x1<x2;
	if(compOp==GT_OP)
		return x1>x2;
	if(compOp==LE_OP)
		return x1<=x2;
	if(compOp==GE_OP)
		return x1>=x2;
	if(compOp==NE_OP)
		return x1!=x2;
	if(compOp==NO_OP)
		return true;
	return false;
}
bool satisfies(CompOp compOp,void *value,void *attr_value,int attr_type,int length)
{
	if(compOp==NO_OP)
		return true;
	if(attr_type==0)
		return compare(*(int *)attr_value,*(int *)value,compOp);
	if(attr_type==1)
		return compare(*(int *)attr_value,*(int *)value,compOp);
	if(attr_type==2)
	{
		if(memcmp(value, attr_value, length) != 0)
			return false;
		else
			return true;
	}
}

RBFM_ScanIterator::RBFM_ScanIterator(){
	rbfm=RecordBasedFileManager::instance();
}

RC RBFM_ScanIterator::getNextRecord(RID &rid, void *data)
{


	int numNullBytes=ceil(double(attributeNames.size())/8);
	unsigned char *nullbytes = (unsigned char *) malloc(numNullBytes);
	memset(nullbytes, 0, numNullBytes);

	void* attribute_data = malloc(PAGE_SIZE);
	void* condition_data = malloc(PAGE_SIZE);
	AttrType this_type;
	int offset = numNullBytes;
	int var_char_length;

	int current_bit;
	bool record_satisfies=false;

	RC returnVal=0;
	void* directory_buffer=malloc(PAGE_SIZE);
	void* header_buffer=malloc(PAGE_SIZE);

	int directory_page_num=rbfm->directory_page(curr_rid.pageNum);
	int header_page_num=rbfm->get_header_page_num(curr_rid.pageNum);

	fileHandle.readPage(directory_page_num,directory_buffer);
	fileHandle.readPage(header_page_num,header_buffer);


	while(!record_satisfies) {
		int record_length = *(int *)((char *)directory_buffer+(curr_rid.slotNum*8));
		int record_offset = *(int *)((char *)directory_buffer+(curr_rid.slotNum*8)+4);
		if(record_length!=9999 && record_offset!=9999) {
			if(compOp!=NO_OP) {
				returnVal=rbfm->readAttribute(fileHandle,recordDescriptor,curr_rid,conditionAttribute,condition_data);
			}
			else {
				returnVal=0;
			}
		}
		else {
			returnVal=-1;
			record_satisfies=false;
		}
		if(returnVal != -1) {
			if(compOp!=NO_OP) {
				for(int j=0;j<recordDescriptor.size();j++) {
					if(recordDescriptor[j].name==conditionAttribute){
						switch(recordDescriptor[j].type) {
							case TypeReal:
								record_satisfies=compare(*(int*)((char*)condition_data+1),*(int *)value,compOp);
								break;
							case TypeInt:
								record_satisfies=compare(*(int*)((char*)condition_data+1),*(int *)value,compOp);
								break;
							case TypeVarChar:
								int length = *(int *)((char*)condition_data+1);
								if(memcmp((char*)value, (char*)condition_data+5, length) != 0)
									record_satisfies=false;
								else
									record_satisfies=true;
								break;
						}
					}
				}
			} else {
				record_satisfies=true;
			}
		}

		if(record_satisfies) {
			for(int i=0;i<attributeNames.size();i++) {
				for(int j=0;j<recordDescriptor.size();j++) {
					if(recordDescriptor[j].name==attributeNames[i])  {
						this_type = recordDescriptor[j].type;
					}
				}
				current_bit = pow(2.0,((numNullBytes*8)-i)-1);
				returnVal=rbfm->readAttribute(fileHandle,recordDescriptor,curr_rid,attributeNames[i],attribute_data);
				if(returnVal==0) {
					switch(this_type) {
						case TypeReal:
							memcpy((char*)data+offset, (char*)attribute_data+1, 4);
							offset = offset + 4;
							break;
						case TypeInt:
							memcpy((char*)data+offset, (char*)attribute_data+1, 4);
							offset = offset + 4;
							break;
						case TypeVarChar:
							memcpy((char *)data+offset,(char *)attribute_data+1,4);
							offset = offset + 4;
							var_char_length=*(int *)((char*)attribute_data+1);
							memcpy((char *)data+offset,(char *)attribute_data+5, var_char_length);
							offset = offset + var_char_length;
							break;
					}
				} else {
					nullbytes[0] = nullbytes[0] | current_bit;
				}
			}
			memcpy((char *)data,nullbytes,numNullBytes);
			rid.slotNum=curr_rid.slotNum;
			rid.pageNum=curr_rid.pageNum;
		}

		curr_rid.slotNum++;
		if(curr_rid.slotNum > 511) {
			curr_rid.pageNum++;
			curr_rid.slotNum=0;
			int index = rbfm->free_space_index(curr_rid.pageNum);
			if(9999==*(int*)((char*)header_buffer+index)) {
				record_satisfies=true;
				returnVal=RBFM_EOF;
			}
			directory_page_num=rbfm->directory_page(curr_rid.pageNum);
			header_page_num=rbfm->get_header_page_num(curr_rid.pageNum);

			fileHandle.readPage(directory_page_num,directory_buffer);
			fileHandle.readPage(header_page_num,header_buffer);
		}
	}
	free(attribute_data);
	free(condition_data);
	attribute_data=NULL;
	condition_data=NULL;
	return returnVal;
}
RC RBFM_ScanIterator::close() {
	rbfm->closeFile(fileHandle);
}
