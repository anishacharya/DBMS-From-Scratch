#include<iostream>
#include "rbfm.h"
#include<cstdlib>
#include<cstdio>
#include<cstring>
#include <cstdio>
#include <cstring>
#include <string>
#include <sstream>
#include <stdio.h>
#include <fstream>
#include<vector>

using namespace std;

RecordBasedFileManager* RecordBasedFileManager::_rbf_manager = 0;

RecordBasedFileManager* RecordBasedFileManager::instance()
{
    if(!_rbf_manager)
        _rbf_manager = new RecordBasedFileManager();

    return _rbf_manager;
}

RecordBasedFileManager::RecordBasedFileManager()
{
}

RecordBasedFileManager::~RecordBasedFileManager()
{
}

////////////////////----------- File Manager Calls -----------------

RC RecordBasedFileManager::createFile(const string &fileName)
{
	PagedFileManager *pfm=PagedFileManager::instance();
	RC result= pfm->createFile(fileName.c_str());

/////-------Every time we call the createFile method we will create a Header Page---------/////
	////----- The Design Decision is that to choose 4 bytes at the beginning of the Header Page/
	//	   Directory Page
	////   to hold the no_of_pages in that File --------////

	//FileHandle fileHandle;
	//pfm->openFile(fileName.c_str(), fileHandle);
	//new_header(fileHandle);
	return result;
}

RC RecordBasedFileManager::destroyFile(const string &fileName) {
	PagedFileManager *pfm=PagedFileManager::instance();
		RC result= pfm->destroyFile(fileName.c_str());
		return result;
}

RC RecordBasedFileManager::openFile(const string &fileName, FileHandle &fileHandle) {
	PagedFileManager *pfm=PagedFileManager::instance();
		RC result= pfm->openFile(fileName.c_str(),fileHandle);
		return result;
}

RC RecordBasedFileManager::closeFile(FileHandle &fileHandle) {
	PagedFileManager *pfm=PagedFileManager::instance();
		RC result= pfm->closeFile(fileHandle);
		return result;
}

///////////---------------Record Related----------------------------------

/////Find the length of the incoming record/////
// utility function required for finding the right page with available space////

int record_length(const void *data,int start, const vector <Attribute> &recordDescriptor,int size)
{
unsigned end=0;

for(unsigned i=0;i<recordDescriptor.size() && i<size;i++)

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

///////-------create a Header Page for each File to keep track of all the pages it has---/////////
int RecordBasedFileManager::new_header(FileHandle &fileHandle)
		{
			void * buffer=malloc(PAGE_SIZE);
			int no_of_pages=-1;
			memcpy((char *)buffer,&no_of_pages,sizeof(int));

			fileHandle.appendPage(buffer);
			free(buffer);

			return fileHandle.getNumberOfPages()-1;
		}

/////you may use system-sequenced file organization. That is,
///find the first page with free space large enough to store the record and store
///the record at that location. RID here is the record ID which is used to uniquely
///identify records in a file. An RID consists of: 1) the page number that the record
///resides in within the file, and 2) the slot number that the record resides in within the page.
////The insertRecord method accepts an RID object and fills it with the RID of the record that is target for insertion.


////////---------check available Page with required space to insert the incoming record------////////


int RecordBasedFileManager::available_page(FileHandle &fileHandle,int space)
{
	void * buffer=malloc(PAGE_SIZE);
	int current=0;
		////As per our strategy we are starting to look from 0
		//as no page == -1 in our strategy
	int count=0;
	fileHandle.readPage(current,buffer);
	while(current!=-1)
	{
		count++;
		bool flag=false;
		int ix=0;

		for(int slot=1;slot<=1023;slot++)
			{
				count++;
				if(count>fileHandle.getNumberOfPages()-1)
				{
					flag=true;
					ix=slot;
					break;
				}

				int source=*(int *)((char *)buffer+slot*4);
				if(source>space)
				{
					source=source-space;
					memcpy((char *)buffer+(slot*4),&source,4);
					fileHandle.writePage(current,buffer);
					free(buffer);
					return count;
				}
				count++;
			}


		if(flag==false && (*(int *)buffer)==-1)
		{
			int newHeader=new_header(fileHandle);
			memcpy((char *)buffer,&newHeader,4);
			fileHandle.writePage(current,buffer);
			fileHandle.readPage(newHeader,buffer);
			int val=PAGE_SIZE-8-space;
			memcpy((char *)buffer+4,&val,4);
			val=-1;
			memcpy(buffer,&val,4);
			fileHandle.writePage(newHeader,buffer);
			free(buffer);
			return -1;
		}

		if(flag==true)
		{
			int val=PAGE_SIZE-8-space;
			memcpy((char *)buffer+ix*4,&val,4);
			fileHandle.writePage(current,buffer);
			free(buffer);
			return -1;
		}


		memcpy(&current,(char *)buffer,4);
	}
	free(buffer);
	return -1;
}



//////------------------------------------------------------------/////
//////------------------------------------------------------------/////
//////------------------------------------------------------------/////
//////------------------------------------------------------------/////
RC RecordBasedFileManager::insertRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor,const void *data, RID &rid)
{
	void* buffer=malloc(PAGE_SIZE);
	if(fileHandle.getNumberOfPages()==0)
	    new_header(fileHandle);
	int end =record_length(data,0,recordDescriptor,recordDescriptor.size());
	int page=available_page(fileHandle,end+12);

	if(page!=-1)
	{
	   fileHandle.readPage(page,buffer);
	   unsigned size;
	   unsigned no_of_records;
	   memcpy(&size,(char *)buffer+PAGE_SIZE-4,4);
	   memcpy(&no_of_records,(char *)buffer+PAGE_SIZE-8,4);
	   rid.pageNum=page;
	   for(rid.slotNum=1;rid.slotNum<=no_of_records;rid.slotNum++)
	   {
		   if(*(int *)((char *)buffer+PAGE_SIZE-8-rid.slotNum*4)==-1)
	    		break;
	   }
	   if(rid.slotNum-no_of_records==1)
	    	no_of_records++;

	   *(int *)((char *)buffer+size)=-1;
	   *(int *)((char *)buffer+size+4)=-1;
	   memcpy((char *)buffer+size+8,data,end);
	   memcpy((char *)buffer+PAGE_SIZE-8-rid.slotNum*4,&size,4);
	   size=size+end+sizeof(int)*2;
	   memcpy((char *)buffer+PAGE_SIZE-4,&size,4);
	   memcpy((char *)buffer+PAGE_SIZE-8,& no_of_records,4);
	   fileHandle.writePage(page,buffer);
	   free(buffer);
	   return 0;
	}
	rid.pageNum=fileHandle.getNumberOfPages();
	rid.slotNum=1;
	int size=end+8,no_of_records=1;
	*(int *)buffer=-1;
	*(int *)((char *)buffer+4)=-1;
	memcpy((char *)buffer+8,data,end);
	memcpy((char *)buffer+PAGE_SIZE-4,&size,4);
	size=0;
	memcpy((char *)buffer+PAGE_SIZE-12,&size,4);
	memcpy((char *)buffer+PAGE_SIZE-8,&no_of_records,4);
	fileHandle.appendPage(buffer);
	free(buffer);
	return 0;
}

RC RecordBasedFileManager::readRecord(FileHandle &fileHandle, const vector<Attribute> &recordDescriptor, const RID &rid, void *data) {

	void * buffer=malloc(PAGE_SIZE);

		if(fileHandle.readPage(rid.pageNum,buffer)!=0)
			{

					return -1;
			}

		int no_of_slots=*(int *)((char *)buffer+PAGE_SIZE-2*sizeof(int));

				if(no_of_slots<rid.slotNum)
					{
							free(buffer);
							return -1;
					}

		int offset=*(int *)((char *)buffer+PAGE_SIZE-sizeof(int)*2-rid.slotNum*sizeof(int));

		if(offset==-1)
		{	free(buffer);
			return -1;
		}

		int point_position=(*(int *)((char *)buffer+offset));
		if(point_position!=-1)
		{
		RID newrid;
		newrid.pageNum=*(int *)((char *)buffer+offset);
		newrid.slotNum=*(int *)((char *)buffer+offset+4);
		free(buffer);

		return readRecord(fileHandle,recordDescriptor,newrid,data);
		}


offset+=sizeof(int)*2;
int end=record_length(buffer,offset,recordDescriptor,recordDescriptor.size());
memcpy((char *)data,(char *)buffer+offset,end);
free(buffer);
return 0;
}
///////////////-------------------------------////
//////------------------------------------------------------------/////
//////------------------------------------------------------------/////
//////------------------------------------------------------------/////
RC RecordBasedFileManager::printRecord(const vector<Attribute> &recordDescriptor, const void *data)
{	//cout<<"in print"<<endl;
	int pointer =0;
	for(int i=0;i<recordDescriptor.size();i++)
	{
		string name = recordDescriptor[i].name;
		switch(recordDescriptor[i].type)
		{
		case TypeInt:
				cout << name << ":"<<endl;
				cout<<*(int*)((char *)data+pointer);
				pointer+=4;
				break;

		case TypeReal:
				cout << name << ":"<<endl;
				cout<<*(float*)((char *)data+pointer);
				pointer+=4;
				break;

		case TypeVarChar:

				cout << name << ":"<<endl;
				for(int j=0;j<*(int *)((char *)data+pointer);j++)
					cout<<*((char *)data+pointer+4+j);

				pointer+=*(int *)((char *)data+pointer)+4;
				break;
		}
	}
    return 0;
}


/////////////-----------Project 2 PArts-------------------///////
////////////----------------------------------------------///////

int RecordBasedFileManager::getDataAttr(const vector<Attribute> &recordDescriptor,
		const string attributeName, const void *data,const void *buffer,bool *flag)
{

	flag=false;
	int length_attr=0;
	int i=0;

	for(int i = 0;i < recordDescriptor.size();i++)
	{
		string name = recordDescriptor[i].name;
		AttrType type = recordDescriptor[i].type;
		AttrLength length = recordDescriptor[i].length;

		if(recordDescriptor[i].name==attributeName)
		{
			int offset=record_length(buffer,0,recordDescriptor,i);
			switch(type)
			{
				case TypeInt:
						memcpy((char *)data,(char *)buffer+offset,4);
						return 4;
						break;
				case TypeReal:
						memcpy((char *)data,(char *)buffer+offset,4);
						return 4;
						break;
				case TypeVarChar:
						memcpy((char *)data,(char *)buffer+offset,4);
						memcpy((char *)data+4,(char *)buffer+offset+4,*(int *)data);
						return *((int *)data)+4;
						break;
			}
		}
	}
	if(i==recordDescriptor.size())
	{
		cout<<"Record not found"<<endl;
		return -1;
	}

	return -1;
}

RC RecordBasedFileManager::readAttribute(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const RID &rid,
		const string attributeName, void *data)
{
	void * buffer=malloc(PAGE_SIZE);
	readRecord(fileHandle, recordDescriptor, rid, buffer);
	//int success=readRecord(fileHandle, recordDescriptor, rid, buffer);
	//cout<<"read record success??"<<success<<endl;
	bool flag;
	getDataAttr(recordDescriptor,attributeName,data,buffer,&flag);
	free(buffer);
	return 0;
}


//////----Delete All the Records///////-----

RC RecordBasedFileManager::deleteRecords(FileHandle &fileHandle)
{
		char *fileName = fileHandle.FileName;
		remove(fileName);
		createFile(fileName);

		return 0;
}

/////-----Delete a particular record//////----
/////-------------------------------//////

RC RecordBasedFileManager::deleteRecord(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const RID &rid)
{
	void *buffer=malloc(PAGE_SIZE);
	int pageNum = rid.pageNum;
	int success=fileHandle.readPage(pageNum,buffer);
	if(success!=0)
	{
		cout<<"reading unsuccessful"<<endl;
		free(buffer);
		return -1;
	}
	int slotPoint=*(int *)((char *)buffer+PAGE_SIZE-8-rid.slotNum*4);

	if (slotPoint == -1)
	{
		cout << rid.pageNum << " " << rid.slotNum << endl;
		cout << "the record is already deleted" << endl;
		free(buffer);
		return -1;
	}

	if(*(int *)((char *)buffer+slotPoint)!=-1)
	{

		RID rid0;
		*(int *)((char *)buffer+PAGE_SIZE-8-rid.slotNum*4)=-1;
		rid0.slotNum=*(int *)((char *)buffer+slotPoint)+4;
		rid0.pageNum=*(int *)((char *)buffer+slotPoint);
		free(buffer);
		return deleteRecord(fileHandle,recordDescriptor,rid0);
	}

	*(int *)((char *)buffer+PAGE_SIZE-8-rid.slotNum*4)=-1;
	fileHandle.writePage(pageNum,buffer);
	fileHandle.readPage(pageNum/(PAGE_SIZE/sizeof(int)),buffer);
	(*(int *)((char *)buffer+(pageNum%1024)*4))+=record_length(buffer,*(int *)((char *)buffer+
			PAGE_SIZE-8-rid.slotNum*4)+8,recordDescriptor,recordDescriptor.size())+8;

	fileHandle.writePage(pageNum/1024,buffer);
	free(buffer);
	return 0;
}

RC RecordBasedFileManager::updateRecord(FileHandle &fileHandle,
		const vector<Attribute> &recordDescriptor, const void *data, const RID &rid)
{
	void *buffer=malloc(PAGE_SIZE);
	int pageNum = rid.pageNum;
	int slotNum=rid.slotNum;
	RID rid0;
	RID rid1;
	//int recordSize = record_length(recordDescriptor, buffer);
	void * ptr = buffer + PAGE_SIZE - 4;

	rid0.pageNum=*(int *)((char *)buffer+*(int *)((char *)buffer+PAGE_SIZE-8-4*slotNum));
	rid0.slotNum=*(int *)((char *)buffer+*(int *)((char *)buffer+PAGE_SIZE-8-4*slotNum)+4);

	int ptr0 = *((int *) ptr - 1 - 2*slotNum);
	int len0 = *((int *) ptr - 1 - 2*slotNum + 1);




	return -1;
}

void swap(int *a,int*b)
{
int temp=*a;
*a=*b;
*b=temp;
}


