
#include "ix.h"
#include<iostream>
#include<cstdlib>
#include<string>
#include<cstdio>
#include <string.h>
#include<stdio.h>
#include<climits>
#include<stdlib.h>
#include<math.h>
#include<functional>
#include<cstring>
#include<sstream>

using namespace std;
unsigned int rc,wc,ac=0;
//unsigned int metadata_page_no=0;



IndexManager* IndexManager::_index_manager = 0;

IndexManager* IndexManager::instance()
{
	if(!_index_manager)
		_index_manager = new IndexManager();

	return _index_manager;
}

IndexManager::IndexManager()
{
}

IndexManager::~IndexManager()
{
}

int utility_hash(const string &fileName,const unsigned &numberOfPages) //// creates the header of Metadata File
{
	int success=0;
	void * buffer=malloc(PAGE_SIZE);

	//	void * buffer1=malloc(PAGE_SIZE);

	PagedFileManager *pfm=PagedFileManager::instance();
	string name=fileName;
	name.append("_hash_utility");
	success=success|pfm->createFile(name.c_str());

	*((int *)buffer)=numberOfPages;   //// N==initial no. of buckets
	*((int *)buffer+1)=0; //// level--level of linear hashing
	*((int *)buffer+2)=0; //// next---split bucket number

	*((int *)buffer+3)=0; //// next empty page
	*((int *)buffer+4)=0;    //// no of overflow pages with vacant
	///Required when looking for an vacant page to be used as new overflow page


	//FileHandle fileHandle1;
	//	success=success|fileHandle1.appendPage(buffer);

	IXFileHandle ixfileHandle;
	success=success|pfm->openFile(name.c_str(),ixfileHandle.fileHandle1);

	success=success|ixfileHandle.fileHandle1.appendPage(buffer);

	free(buffer);
	return success;
	/// the last 4 bytes contains the pointer to the
	/// next utility page in case the first metadata page overflows
	//*((int *)buffer+PAGE_SIZE/4-1)=NULL;

	//for(int i=1;i<numberOfPages;i++)
	//{
	/////// allocate 8 bytes for storing the info about each
	/////// overlapping buckets
	//////  each bucket
	//*((int *)buffer1+2+2*i)=0;
	//}
	//FileHandle fileHandle;
	/* pfm->openFile(name.c_str(),fileHandle);
		cout<<"1";
		fileHandle.writePage(0, buffer);
		cout<<"2";
		fseek(fileHandle.file,0,SEEK_SET);
		fwrite(buffer,PAGE_SIZE,1,fileHandle.file);
	 */

}



RC IndexManager::createFile(const string &fileName, const unsigned &numberOfPages)
{
	cout << "Inside createfile function" << endl;


	//// Second 4 bytes store number of buckets/ primary pages currently

	int success=0;
	PagedFileManager *pfm=PagedFileManager::instance();
	success=success| pfm->createFile(fileName.c_str());

	success=success|utility_hash(fileName,numberOfPages);
	cout << "Metadata file created" << endl;
	//FileHandle fileHandle;

	IXFileHandle ixfileHandle;

	//success=success| pfm->openFile(fileName.c_str(),fileHandle);
	//for(int i=0;i<numberOfPages;i++)
	//{
	//success=success|fileHandle.appendPage(buffer);
	//}

	//success=success| pfm->openFile(fileName.c_str(),ixfileHandle.fileHandle0);
	//ixfileHandle.fileHandle0.appendPage(buffer_header);

	/////// create primary pages ////-----new function now -------
	void * buffer=malloc(PAGE_SIZE);

	//void * buffer_header=malloc(PAGE_SIZE);
	*((int *)buffer)=0;
	///// First 4 bytes keep the address of its first overflow page
	*((int *)buffer+1)=PAGE_SIZE-8;
	///// Store the available amount of space

	for(int i=0;i<numberOfPages;i++)
	{
		success=success| pfm->openFile(fileName.c_str(),ixfileHandle.fileHandle0);
		success=success|ixfileHandle.fileHandle0.appendPage(buffer);
		success=success| pfm->closeFile(ixfileHandle.fileHandle0);
	}
	cout << "Appended all the buckets" << endl;
	closeFile(ixfileHandle);
	free(buffer);


	return success;
}

RC IndexManager::createnewBucket(FileHandle fileHandle)    //// required for split
{
	int success=0;
	void * buffer=malloc(PAGE_SIZE);

	//void * buffer_header=malloc(PAGE_SIZE);
	*((int *)buffer)=0;
	///// First 4 bytes keep the address of its first overflow page
	*((int *)buffer+1)=PAGE_SIZE-8;
	///// Store the available amount of space
	success=success|fileHandle.appendPage(buffer);
	free(buffer);
	return success;
}

RC IndexManager::createnewOverflow(FileHandle fileHandle)    //// required for split
{
	int success=0;
	void * buffer=malloc(PAGE_SIZE);

	//void * buffer_header=malloc(PAGE_SIZE);
	*((int *)buffer)=0;
	///// First 4 bytes keep the pagenum in metadata file of its first overflow page
	*((int *)buffer+1)=PAGE_SIZE-16;
	///// Store the available amount of space
	*((int *)buffer+3)=0;
	///Store the location of next empty page if it is empty and if any more available
	*((int *)buffer+4)=fileHandle.getNumberOfPages()-1;
	///store the pageNumber of this page in the metadata file
	success=success|fileHandle.appendPage(buffer);



	free(buffer);
	return success;
}

RC IndexManager::destroyFile(const string &fileName)
{
	int success=0;
	string name=fileName;
	name.append("_hash_utility");
	PagedFileManager *pfm=PagedFileManager::instance();
	success=success| pfm->destroyFile(fileName.c_str());
	success=success| pfm->destroyFile(name.c_str());
	return success;
}

RC IndexManager::openFile(const string &fileName, IXFileHandle &ixFileHandle)
{
	int success=0;
	PagedFileManager *pfm=PagedFileManager::instance();
	//FileHandle fileHandle0;
	//FileHandle fileHandle1;
	string name=fileName;
	name.append("_hash_utility");
	success=success| pfm->openFile(fileName.c_str(),ixFileHandle.fileHandle0);
	success=success| pfm->openFile(name.c_str(),ixFileHandle.fileHandle1);

	return success;
}

RC IndexManager::closeFile(IXFileHandle &ixfileHandle)
{
	int success=0;
	PagedFileManager *pfm=PagedFileManager::instance();
	success=success| pfm->closeFile(ixfileHandle.fileHandle0);
	success=success| pfm->closeFile(ixfileHandle.fileHandle1);
	return success;
}


////////////////////////////////////////////////

RC IndexManager:: bucketnum(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
	//int bucketnum=0;
	///get the hashed key
	int h=hash(attribute,key);

	///get N ---- number of primary buckets
	//unsigned N=0;
	//getNumberOfPrimaryPages(ixfileHandle,N);

	///get level
	///get N ---- number of primary buckets - constant
	///get the bucket number pointed by next
	void* buffer=malloc(PAGE_SIZE);
	ixfileHandle.fileHandle1.readPage(1,buffer);

	//unsigned int rc,wc,ac=0;
	//ixfileHandle.fileHandle0.collectCounterValues(rc,wc,ac);
	//cout<<rc<<endl<<wc<<endl<<ac<<endl;

	int N=*((int *)buffer);
	int L=*((int *)buffer+1);
	int next=*((int *)buffer+2);


	int H=h%(N*(2^L));

	if(H<next)
		H= h%(N*(2^(L+1)));


	unsigned N_current;
	getNumberOfPrimaryPages(ixfileHandle,N_current);

	return H;

	//ref:Wikipedia
}

int IndexManager::Record_length(const Attribute &attribute, const void *data)
{
	int end = 0;
	//for(unsigned i = 0; i < attribute.size(); i++){
	AttrType type =attribute.type;
	if(type == TypeVarChar)
	{
		int stringSize = *((char *)data + end);
		end += 4;
		end += stringSize;
	}
	else if(type == TypeInt)
	{
		end += attribute.length;
	}
	else if(type == TypeReal)
	{
		end += attribute.length;
	}

	return end;
}
/*
RC IndexManager::insertEntryinBucket(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key,
		const RID &rid,void *buffer,int Bucket)
{
	success=success|ixfileHandle.fileHandle0.readPage(Bucket-1,buffer);   ///coz PageNum starts from 0
	if(*((int *)buffer)==0)
		///check for overflow pages of the current bucket
	{

		int length_of_key=Record_length(attribute,key);   ///get the length of the record
		//int length_of_key=attribute.length;
		int available_space=*((int *)buffer+1);   //// check how much space is available in that page

		if(available_space>length_of_key+8)     //// because we need to insert <key,rid> pair ---rid=4+4=8
		{
			//PagedFileManager *pfm=PagedFileManager::instance();


			////write the record
			void* ptr=((void *)buffer+PAGE_SIZE-available_space); //////
			memcpy(ptr,key,length_of_key);
			memcpy(ptr+length_of_key,&rid.pageNum,4);
			memcpy(ptr+length_of_key+4,&rid.slotNum,4);


			//fseek(,*((int *)buffer+1),SEEK_SET);
			//fwrite
			//*((int *)buffer+1)+=length_of_key/4;        ///update the available space
			available_space-=length_of_key+8;
 *((int *)buffer+1)=available_space;
			success=success|ixfileHandle.fileHandle0.writePage(Bucket-1,buffer);
			//unsigned int rc,wc,ac=0;
			//ixfileHandle.fileHandle0.collectCounterValues(rc,wc,ac);
			//cout<<rc<<endl<<wc<<endl<<ac<<endl;  ///working ---checked
		}
		else
		{
			/////overflow page

			///// check if any empty page available in metadata page which can be used as overflow page
			void *buffer0=malloc(PAGE_SIZE);
			success=success|ixfileHandle.fileHandle1.readPage(1,buffer0);
			if(*((int *)buffer0+4)>0)       ////// implies empty pages available as this stores no. of empty pages
			{



				//// go to this page pointed and insert
			}
			///// else create a new page in Utility Hash
			else
			{
				void*data=malloc(PAGE_SIZE);
 *((int *)data)=0;////next overflow page of this bucket
 *((int *)data+1)=1024-2;
				success=success|ixfileHandle.fileHandle1.appendPage(data);
				///fseek
				///fwrite
			}

		}

	}


	else
	{

		//go to the overflow page
	}
	return -1;
}
 */


RC IndexManager::write_in_a_OverflowBucket(int available_space,IXFileHandle &ixfileHandle, const Attribute &attribute,
		const void *key, const RID &rid,void *buffer,int length_of_key,int pagenumber_in_metadata)
{

	int success=0;
	void* ptr=((void *)buffer+PAGE_SIZE-available_space);
	memcpy(ptr,key,length_of_key);
	memcpy(ptr+length_of_key,&rid.pageNum,4);
	memcpy(ptr+length_of_key+4,&rid.slotNum,4);
	available_space-=length_of_key+8;			///update the available space
	*((int *)buffer+1)=available_space;
	return success|ixfileHandle.fileHandle1.writePage(pagenumber_in_metadata,buffer);

}

RC IndexManager::write_in_a_bucket(int available_space,IXFileHandle &ixfileHandle,
		const Attribute &attribute, const void *key, const RID &rid,void* buffer,int length_of_key,int Bucket)
{
	int success=0;
	//PagedFileManager *pfm=PagedFileManager::instance();
	////write the record
	void* ptr=((void *)buffer+PAGE_SIZE-available_space);
	memcpy(ptr,key,length_of_key);
	memcpy(ptr+length_of_key,&rid.pageNum,4);
	memcpy(ptr+length_of_key+4,&rid.slotNum,4);
	//fseek(,*((int *)buffer+1),SEEK_SET);
	//fwrite
	//*((int *)buffer+1)+=length_of_key/4;        ///update the available space
	available_space-=length_of_key+8;			///update the available space
	*((int *)buffer+1)=available_space;
	return success|ixfileHandle.fileHandle0.writePage(Bucket-1,buffer);
	//unsigned int rc,wc,ac=0;
	//ixfileHandle.fileHandle0.collectCounterValues(rc,wc,ac);
	//cout<<rc<<endl<<wc<<endl<<ac<<endl;  ///working ---checked

}

RC IndexManager::insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
	int Bucket=bucketnum(ixfileHandle,attribute,key,rid);
	cout<<"Bucket number"<<Bucket<<endl;                            /// so we get the required bucket number
	/// check overflow:here bucket is a page -so check if that page has space for this record
	void *buffer=malloc(PAGE_SIZE);
	int success=0;
	//PagedFileManager *pfm=PagedFileManager::instance();
	//success=success|pfm->readPage()
	//unsigned int rc,wc,ac=0;
	//ixfileHandle.fileHandle0.collectCounterValues(rc,wc,ac);
	//cout<<rc<<endl<<wc<<endl<<ac<<endl;     //////readcounter=1 ---checked.
	//	success=success|insertEntryinBucket(ixfileHandle,attribute,key,rid,buffer,Bucket);
	success=success|ixfileHandle.fileHandle0.readPage(Bucket-1,buffer);   ///coz PageNum starts from 0

	int next_overlow=*((int *)buffer);
	int available_space=*((int *)buffer+1);   //// check how much space is available in that page
	int length_of_key=Record_length(attribute,key);   ///get the length of the record
	int space_reqd=length_of_key+8;

	//CASE- 1 : space Available
	if(available_space>=space_reqd)     //// because we need to insert <key,rid> pair ---rid=4+4=8
	{
		success=success|write_in_a_bucket(available_space,ixfileHandle,
				attribute, key,rid, buffer,length_of_key,Bucket);
		return success;
		/*
			//PagedFileManager *pfm=PagedFileManager::instance();
			////write the record
			void* ptr=((void *)buffer+PAGE_SIZE-available_space); //////
			memcpy(ptr,key,length_of_key);
			memcpy(ptr+length_of_key,&rid.pageNum,4);
			memcpy(ptr+length_of_key+4,&rid.slotNum,4);
			//fseek(,*((int *)buffer+1),SEEK_SET);
			//fwrite
			//*((int *)buffer+1)+=length_of_key/4;        ///update the available space
			available_space-=length_of_key+8;			///update the available space
		 *((int *)buffer+1)=available_space;
			success=success|ixfileHandle.fileHandle0.writePage(Bucket-1,buffer);
			//unsigned int rc,wc,ac=0;
			//ixfileHandle.fileHandle0.collectCounterValues(rc,wc,ac);
			//cout<<rc<<endl<<wc<<endl<<ac<<endl;  ///working ---checked
		 */
	}
	//CASE-2 : No Space so have to create an overflow page/use overflow
	else
	{
		if(next_overlow==0)
		{
			//check if vacant page available in metadata
			void *buffer0=malloc(PAGE_SIZE);
			success=success|ixfileHandle.fileHandle1.readPage(0,buffer0);/// Read the header page of Metadata File

			int N=*((int *)buffer0);
			int L=*((int *)buffer0+1);
			int next=*((int *)buffer0+2);
			int first_empty_pagenum=*((int *)buffer0+3);
			int available_empty_pages=*((int *)buffer0+4);


			if(available_empty_pages==0) /// no empty page to use
			{
				//create new overflow page
				success=success|createnewOverflow(ixfileHandle.fileHandle1);
				///update the primary bucket
				next_overlow=ixfileHandle.fileHandle1.getNumberOfPages()-1;
				*((int *)buffer)=next_overlow;
				/////insert in the overflow page
				void *buffer0=malloc(PAGE_SIZE);
				success=success|ixfileHandle.fileHandle1.readPage(next_overlow,buffer0);
				int available_space=*((int *)buffer0+1);
				success=success|write_in_a_OverflowBucket(available_space,ixfileHandle,attribute,
						key,rid,buffer0,length_of_key,next_overlow);

				/////update the actual bucket to point to this new overflow page
				int pagenum=*((int *)buffer0+3);
				*((int *)buffer)=pagenum;
				success=success|ixfileHandle.fileHandle0.writePage(Bucket-1,buffer);

				free(buffer0);
				free(buffer);
				return success;

			}
			else
			{
				void* buffer1=malloc(PAGE_SIZE);
				success=success|ixfileHandle.fileHandle1.readPage(first_empty_pagenum,buffer1);

				int available_space=*((int *)buffer1+1);
				int page_num=*((int *)buffer1+3);
				int next_empty=*((int *)buffer1+2);

				*((int *)buffer0+3)=next_empty;
				success=success|ixfileHandle.fileHandle1.writePage(0,buffer0);    //// update the metadata header

				success=success|write_in_a_OverflowBucket(available_space,ixfileHandle,attribute,
						key,rid,buffer1,length_of_key,page_num);

				/////update the actual bucket to point to this new overflow page
				int pagenum=*((int *)buffer1+3);
				*((int *)buffer)=pagenum;
				success=success|ixfileHandle.fileHandle0.writePage(Bucket-1,buffer);

				free(buffer);
				free(buffer1);
				return success;
				//get the pageNum of the first available empty page
			}



		}

	}

	return success;
}

///////-------------------TO DO -------------------/////******
RC IndexManager:: loopy_insert_overflow(int Bucket,IXFileHandle ixfileHandle,const Attribute &attribute, const void *key, const RID &rid)
{

	void *buffer=malloc(PAGE_SIZE);
	int success=0;
	success=success|ixfileHandle.fileHandle0.readPage(Bucket-1,buffer);   ///coz PageNum starts from 0

	int next_overlow=*((int *)buffer);
	int available_space=*((int *)buffer+1);   //// check how much space is available in that page
	int length_of_key=Record_length(attribute,key);   ///get the length of the record
	int space_reqd=length_of_key+8;

	//Corner case : space Available
	if(available_space>=space_reqd)     //// because we need to insert <key,rid> pair ---rid=4+4=8
	{
		success=success|write_in_a_bucket(available_space,ixfileHandle,
				attribute, key,rid, buffer,length_of_key,Bucket);
		return success;
	}
return -1;
}
RC IndexManager::deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute,
		const void *key, const RID &rid)
{
	return -1;
}

unsigned int inthash(int key)
{
	return key*2654435761%(2^32);   /////Knuth's multiplicative method
}
/*
int inthash(int key)
{
  key += ~(key << 15);
  key ^=  (key >>> 10);
  key +=  (key << 3);
  key ^=  (key >>> 6);
  key += ~(key << 11);
  key ^=  (key >>> 16);
  return key;
}
 */
unsigned int PJWHash(const std::string& str)   ///// Ref:  Peter J. Weinberger
{
	unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
	unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
	unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
	unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
	unsigned int hash              = 0;
	unsigned int test              = 0;

	for(std::size_t i = 0; i < str.length(); i++)
	{
		hash = (hash << OneEighth) + str[i];

		if((test = hash & HighBits)  != 0)
		{
			hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return hash;
}

//template <typename T> string tostr(const T& t)
//{
//  ostringstream os;
// os<<t;
//return os.str();
//}
unsigned IndexManager::hash(const Attribute &attribute, const void *key)
{
	switch(attribute.type)
	{
	case 0:
		//std::hash<std::int>h1;
		//return h1(*((int*)key));  //Ref:cplusplus.com
		//int key_0=*((int*)key);
		return inthash(*((int*)key));
		//return key0*2654435761%(2^32);    /////Knuth's multiplicative method
		break;

	case 1:
		//float key1=*((float*)key);
		//ostringstream ss;
		//ss << *((float*)key);
		//string str0(ss.str0());
		//string str=tostr(*((float*)key));
		return PJWHash(*((string*)key));

		//std::hash<std::float>h2;
		//return h2(*((float*)key));
		break;

	case 2:
		//std::hash<std::char>h3;
		//return h3(*((char*)key));
		//char key2=*((char*)key);
		//ostringstream ss1;
		//ss1 << *((char*)key);
		//string str1(ss1.str());
		//string str1=tostr(*((char*)key));
		return PJWHash(*((string*)key));
		break;
	}
	return 0;
}




RC IndexManager::printIndexEntriesInAPage(IXFileHandle &ixfileHandle, const Attribute &attribute, const unsigned &primaryPageNumber) 
{
	return -1;
}

RC IndexManager::getNumberOfPrimaryPages(IXFileHandle &ixfileHandle,
		unsigned &numberOfPrimaryPages)
{

	//void* buffer=malloc(PAGE_SIZE);
	//ixfileHandle.fileHandle0.readPage(1,buffer);

	//numberOfPrimaryPages=*((int *)buffer+1);
	numberOfPrimaryPages=ixfileHandle.fileHandle0.getNumberOfPages();

	return 0;
}

RC IndexManager::getNumberOfAllPages(IXFileHandle &ixfileHandle, unsigned &numberOfAllPages) 
{


	void* buffer=malloc(PAGE_SIZE);
	ixfileHandle.fileHandle1.readPage(1,buffer);

	numberOfAllPages=*((int *)buffer+1);

	unsigned int temp=0;
	getNumberOfPrimaryPages(ixfileHandle,temp);
	numberOfAllPages+=temp;


	return 0;

	//return -1;
}


RC IndexManager::scan(IXFileHandle &ixfileHandle,
		const Attribute &attribute,
		const void      *lowKey,
		const void      *highKey,
		bool			lowKeyInclusive,
		bool        	highKeyInclusive,
		IX_ScanIterator &ix_ScanIterator)
{
	return -1;
}

IX_ScanIterator::IX_ScanIterator()
{
}

IX_ScanIterator::~IX_ScanIterator()
{
}

RC IX_ScanIterator::getNextEntry(RID &rid, void *key)
{
	return -1;
}

RC IX_ScanIterator::close()
{
	return -1;
}


IXFileHandle::IXFileHandle()
{
	//util_pagenum=0;
}

IXFileHandle::~IXFileHandle()
{
}

RC IXFileHandle::collectCounterValues(unsigned &readPageCount,
		unsigned &writePageCount, unsigned &appendPageCount)
{
	//IXFileHandle ixfileHandle;
	unsigned int read_primary=0;
	unsigned int write_primary=0;
	unsigned int append_primary=0;

	unsigned int read_metadata=0;
	unsigned int write_metadata=0;
	unsigned int append_metadata=0;

	//ixfileHandle.fileHandle0.collectCounterValues(read_primary,write_primary,append_primary);
	//ixfileHandle.fileHandle1.collectCounterValues(read_metadata,write_metadata,append_metadata);

	//readPageCount=read_primary+read_metadata;
	//writePageCount+=write_primary+write_metadata;
	//appendPageCount+=append_metadata+append_primary;

	//ixfileHandle.fileHandle0.collectCounterValues(readPageCount,writePageCount,appendPageCount);
	fileHandle0.collectCounterValues(read_primary,write_primary,append_primary);
	fileHandle1.collectCounterValues(read_metadata,write_metadata,append_metadata);

	readPageCount=read_primary+read_metadata;
	writePageCount=write_primary+write_metadata;
	appendPageCount=append_primary+append_metadata;
	cout<<readPageCount<<endl;
	return 0;
}

void IX_PrintError (RC rc)
{
}
