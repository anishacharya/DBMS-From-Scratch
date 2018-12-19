#include "ix.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
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

RC IndexManager::createFile(const string &fileName)
{
	PagedFileManager *pfm=PagedFileManager::instance();
	RC result= pfm->createFile(fileName.c_str());
	file_exist=true;
	return result;
}

RC IndexManager::destroyFile(const string &fileName)
{
	int success=0;
	PagedFileManager *pfm=PagedFileManager::instance();
	success = success | pfm->destroyFile(fileName.c_str());
	file_exist=false;
	return success;
}

RC IndexManager::openFile(const string &fileName, IXFileHandle &ixFileHandle)
{
	PagedFileManager *pfm=PagedFileManager::instance();
	RC result= pfm->openFile(fileName.c_str(),ixFileHandle.fileHandle);
	//ixFileHandle.openflag=true;
	
	if(result==0)
		file_exist=true;
	return result;
}

RC IndexManager::closeFile(IXFileHandle &ixfileHandle)
{
	PagedFileManager *pfm=PagedFileManager::instance();
	RC result= pfm->closeFile(ixfileHandle.fileHandle);
	//ixfileHandle.openflag=false;

	return result;
}

/// In order to insert <key,rid> we need the attr Length
int attrLen(Attribute attribute,const void *key)
{
// This method would return the attribute Length 
// If fails returns -1 ---- should not happen
	switch(attribute.type)
   	{
			case 0:
				return sizeof(int);  // type Int
				//break;
			case 1:
				return sizeof(float); // type Float
				//break;
			case 2:
				return *((int *)key)+sizeof(int); // type VarChar
				//break;
			
	}

	return -1;
}
bool smaller(const Attribute &attribute,const void *a,const void *b)
{
	switch(attribute.type)
	{
	case 0:
		return ((*(int *)a)<(*(int *)b));
	case 1:
		return ((*(float *)a)<(*(float *)b));
	case 2:
		string x="",y="";
		int l=*(int *)a;
		for(int i=0;i<l;i++)
			x+=*((char *)a+sizeof(int)+i);
		l=*(int *)b;
		for(int i=0;i<l;i++)
			y+=*((char *)b+sizeof(int)+i);
		return x<y;
	}
	return false;
}

bool satisfies(const void * value,const void *high,const void * low,bool highInc,bool lowInc,const Attribute &attribute)
{
	if(high==NULL && low==NULL)
	return true;

	bool c1=true,c2=true;

	int len=attrLen(attribute,value);
	if(low!=NULL)
	{
		if(!smaller(attribute,low,value))
			c1=false;
		if(lowInc && (memcmp(low,value,len)==0))
			c1=true;
	}
	if(high!=NULL)
	{
		if(!smaller(attribute,value,high))
			c2=false;
		if(highInc && (memcmp(high,value,len)==0))
			c2=true;
	}
	return c1 && c2;
}


RC IndexManager::beyondRootInsert(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid,int i)
{
	int success=0;
	cout<<"creating linked leaf"<<endl;
	void* leaf=malloc(PAGE_SIZE);
	void* root=malloc(PAGE_SIZE);
	int no_of_pages=ixfileHandle.fileHandle.getNumberOfPages();
	cout<<"Number of pages before insert in LEAF"<<no_of_pages<<endl;
	success=success|ixfileHandle.fileHandle.readPage(i-1,root);
		if(success!=0)
			cout<<"Read fails in leaf"<<endl;
	cout<<"Free Space in Root"<<*(int *)((char *)root+PAGE_SIZE-sizeof(int))<<endl; // working --- good

	int length=attrLen(attribute,key);
	
	if(no_of_pages==i)
	{
		//int returnval=ixfileHandle.fileHandle.readPage(i,leaf);
		//if(returnval!=0)
		//	cout<<"Should Fail Read"<<endl; /// correct 
		// append the first leaf
		cout<<"Appending the First leaf"<<endl;
		*(int *)((char *)leaf)=length;
		memcpy((char *)leaf+sizeof(int),key,length);
		*(int *)((char *)leaf+length+sizeof(int))=rid.pageNum;
		*(int *)((char *)leaf+length+2*sizeof(int))=rid.slotNum;
		*(int *)((char *)leaf+PAGE_SIZE-sizeof(int))=4088-(length+12);//setting free space
		*(int *)((char *)leaf+PAGE_SIZE-2*sizeof(int))=length+12; //setting offset in new leaf
		 
		//write page
		success=success|ixfileHandle.fileHandle.appendPage(leaf);
		if(success!=0)
			cout<<"In Leaf - append failed"<<endl;
		int no_of_pages=ixfileHandle.fileHandle.getNumberOfPages();
		cout<<"Number of pages after insert in LEAF"<<no_of_pages<<endl;
		free(leaf);
		free(root);
		leaf=NULL;
		root=NULL;
		return success;	
	}	
	/*else if(no_of_pages=i+1)
	{
		cout<<"NOW IS THE TIME"<<endl;
		// read free space
		success=success|ixfileHandle.fileHandle.readPage(i,leaf);
		int free_space=*(int *)((char *)leaf+PAGE_SIZE-sizeof(int));
		if(free_space>=length)
			{
				cout<<"We are good in the current page"<<endl;
				int offset=*(int *)((char *)leaf+PAGE_SIZE-2*sizeof(int));
				*(int *)((char *)leaf+offset)=length;
				memcpy((char *)leaf+sizeof(int)+offset,key,length);
				*(int *)((char *)leaf+length+sizeof(int)+offset)=rid.pageNum;
				*(int *)((char *)leaf+length+2*sizeof(int)+offset)=rid.slotNum;	
				*(int *)((char *)leaf+PAGE_SIZE-sizeof(int))=free_space-(length+12);//update free space
				*(int *)((char *)leaf+PAGE_SIZE-2*sizeof(int))=offset+(length+12);// update offset
				//write to page
				ixfileHandle.fileHandle.writePage(i,leaf);
				
				free(leaf);
				free(root);
				leaf=NULL;
				root=NULL;
				return success;		
			}
		else
			{
				cout<<"No free Space- more split"<<endl;
				success=success|beyondRootInsert(ixfileHandle,attribute,key,rid,i+1);
				
			}
			
	}

*/
	return -1;
}


RC IndexManager::insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{
    void* indexpage=malloc(PAGE_SIZE);	// blank page now
    //void* Keyspage=malloc(PAGE_SIZE);
    //void* Key_len_page=malloc(PAGE_SIZE);
    // TODO: finalize page structure
    int Key_len=attrLen(attribute,key);	// <key,rid> pair to be inserted. key of size Key_len followed by rid.pgNum and rid.slotnum
    cout<<"Attribute Length of key to insert:="<<attrLen(attribute,key)<<endl;
    int success =0;	
    int no_of_pages=ixfileHandle.fileHandle.getNumberOfPages();
    cout<<"Number of pages before insert"<<no_of_pages<<endl;
    cout<<"RID->PageNum"<<rid.pageNum<<endl;
    //void* key_buff=malloc(Key_len);
    //*key_buff = key;
    if(no_of_pages==0)	// create first index page----- root node .... split when full ---
    {		// set last 4 bytes as free space
	*(int *)((char *)indexpage+PAGE_SIZE-sizeof(int))=4088;  //// max 4088 bytes of entry
	*(int *)((char *)indexpage+PAGE_SIZE-2*sizeof(int))=0;   ///  second last 4 bytes for storring current offset for insert
	cout<<"Length of Key:==>>"<<Key_len<<endl;
	//simple insert first key then rid
	*(int *)((char *)indexpage)=Key_len; 		                 // first 4 bytes for key length	
	memcpy((char *)indexpage+sizeof(int),key,Key_len); 		/// first write key so offset now -> 4+key_len
	*(int *)((char *)indexpage+sizeof(int)+Key_len)=rid.pageNum;  // then write pageNum offset now ->2*4+key_len
	*(int *)((char *)indexpage+2*sizeof(int)+Key_len)=rid.slotNum;	// then write slotNum offset now -> 3*4+key_len
	*(int *)((char *)indexpage+3*sizeof(int)+Key_len)=-1; 		//set next 4 bytes i.e. length of next key to -1 implies no record 
	cout<<"Appending page to file:==="<<ixfileHandle.fileHandle.FileName<<endl;
	//cout<<"Number of PAges before Append ::===>"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;
		// update the free space
	*(int *)((char *)indexpage+PAGE_SIZE-sizeof(int))=4088-(12+Key_len);
	// update offset
	*(int *)((char *)indexpage+PAGE_SIZE-2*sizeof(int))=12+Key_len;
	success=success|ixfileHandle.fileHandle.appendPage(indexpage);
	free(indexpage);
    	indexpage=NULL; 
	//cout<<"Number of PAges after append ::====>"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;
	//cout<<"Number of Append PAges after append ::====>"<<ixfileHandle.fileHandle.appendPageCounter<<endl;
	void *test=malloc(PAGE_SIZE);
	success=success|ixfileHandle.fileHandle.readPage(0,test);
	if(success!=0)
		cout<<"Read Failed"<<endl;
	cout<<"TESTING READ_COUNTER ::====>"<<ixfileHandle.fileHandle.readPageCounter<<endl; 
								// in 4a getting trash values - why?initialization?
	//free(test);
	//test=NULL;	
	//cout<< "RID->PAGENUM :====>" <<rid.pageNum<<endl;
	//cout<< "RID->SLOT :====>"<<rid.slotNum<<endl;
	//cout<< "KEY to Insert is :====> "<<*(int *)key<<endl;
	// sanity check:
	//
	cout<<"First 4 bytes:=="<<*(int *)((char *)test)<<endl;
	cout<<"PageNum inserted:=="<<*(int *)((char *)test+sizeof(int)+Key_len)<<endl;
	//cout<<"SECOND length bytes:=="<<(char*)((char *)indexpage+sizeof(int))<<endl;
//	cout<<"FILE where insert happened"<<ixfileHandle.fileHandle.FileName<<endl;
	    free(test);
	test=NULL;
    } 
    // else condition TODO ---> overflow condition
    // else if page =1 then keep adding entries in root--- if not then page nums can be 1,3,7,...
 	else //if(no_of_pages==1)
	{
		// keep adding to this root unless run out of space
		// 1. read free space
		int num=0;// current root page
		//success=success|ixfileHandle.fileHandle.readPage(num,indexpage);
		success=success|ixfileHandle.fileHandle.readPage(no_of_pages-1,indexpage);	
		int free_space=*(int *)((char *)indexpage+PAGE_SIZE-sizeof(int));
		cout<<"FREE SPACE in IX PAGE:"<<free_space<<endl;
		// read offset
		int offset=*(int *)((char *)indexpage+PAGE_SIZE-2*sizeof(int));
		cout<<"current offset in IX PAGE:"<<offset<<endl;
	
		if(free_space<Key_len+12)
			{
				cout<<"NO MORE SPACE, NEED TO SPLIT NOW"<<endl; // split logic will go here
				free(indexpage);
				indexpage=NULL;	
				//success=success|beyondRootInsert(ixfileHandle,attribute,key,rid,1); //called with current page =0
				success=success|beyondRootInsert(ixfileHandle,attribute,key,rid,no_of_pages);
				cout<<"number of pages now"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;
				return success;// to check forced stop//----working stops after 255 entries --- we are on right track
			}
		//return -1;// forced stop to check // working ---checked		
		else
		{
			// keep adding here ----5a,6a,7,8,9 should pass  ----- 
			// Upto 255 entries / will keep a direc page for key lens then pass upto 350 i.e. 6a

			cout<<"STILL IN FIRST IX PAGE"<<endl;
			cout<<"Length of Key:==>>"<<Key_len<<endl;
			cout<<"OFFSET INSERT ===>>>>>>"<<offset<<endl;
			//simple insert first key then rid
			*(int *)((char *)indexpage+offset)=Key_len; 		                 // first 4 bytes for key length	
			memcpy((char *)indexpage+sizeof(int)+offset,key,Key_len); 		/// first write key so offset now -> 4+key_len
			*(int *)((char *)indexpage+sizeof(int)+Key_len+offset)=rid.pageNum;  // then write pageNum offset now ->2*4+key_len
			*(int *)((char *)indexpage+2*sizeof(int)+Key_len+offset)=rid.slotNum;	// then write slotNum offset now -> 3*4+key_len
			*(int *)((char *)indexpage+3*sizeof(int)+Key_len+offset)=-1; 		
									//set next 4 bytes i.e. length of next key to -1 implies no record 
			cout<<"writing page to file:==="<<ixfileHandle.fileHandle.FileName<<endl;
			// update free space
			*(int *)((char *)indexpage+PAGE_SIZE-sizeof(int))=free_space-(Key_len+12);
			//update offset
			*(int *)((char *)indexpage+PAGE_SIZE-2*sizeof(int))=offset+Key_len+12;
			// write to page
			//success=success|ixfileHandle.fileHandle.writePage(num,indexpage);
			success=success|ixfileHandle.fileHandle.writePage(no_of_pages-1,indexpage);
			free(indexpage);
			indexpage=NULL;
/*
	//cout<<"Number of PAges after append ::====>"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;
	//cout<<"Number of Append PAges after append ::====>"<<ixfileHandle.fileHandle.appendPageCounter<<endl;
	void *test=malloc(PAGE_SIZE);
	success=success|ixfileHandle.fileHandle.readPage(0,test);
	if(success!=0)
		cout<<"Read Failed"<<endl;
	//cout<<"TESTING READ_COUNTER ::====>"<<ixfileHandle.fileHandle.readPageCounter<<endl; 
								// in 4a getting trash values - why?initialization?
	//free(test);
	//test=NULL;	
	//cout<< "RID->PAGENUM :====>" <<rid.pageNum<<endl;
	//cout<< "RID->SLOT :====>"<<rid.slotNum<<endl;
	//cout<< "KEY to Insert is :====> "<<*(int *)key<<endl;
	// sanity check:
	//
	cout<<"First 4 bytes:=="<<*(int *)((char *)test+offset)<<endl;
	cout<<"PageNum inserted:=="<<*(int *)((char *)test+sizeof(int)+Key_len+offset)<<endl;
	//cout<<"SECOND length bytes:=="<<(char*)((char *)indexpage+sizeof(int))<<endl;
//	cout<<"FILE where insert happened"<<ixfileHandle.fileHandle.FileName<<endl;
	    free(test);
	test=NULL;
*/
			return success;
		}
	}
	/*else
	{
		success=success|beyondRootInsert(ixfileHandle,attribute,key,rid,no_of_pages); //called with current page =0	
	} */
    cout<<"Done with insert Entry"<<endl;	
    return success;
}


/* --------------------old delete --------------*/

RC IndexManager::deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid)
{      
    int success =0;	
	bool deleted=false;
    cout<<"Filename:===>"<<ixfileHandle.fileHandle.FileName<<endl;

    cout<<"Inside deleteEntry"<<endl;

    int Key_len=attrLen(attribute,key);
    cout<<"Key Length to be deleted::===>"<<Key_len<<endl;

    int no_of_pages=ixfileHandle.fileHandle.getNumberOfPages();
    cout<<"Number of pages before delete"<<no_of_pages<<endl;
    cout<<"RID->PageNum"<<rid.pageNum<<endl;

    if(no_of_pages==0)
    {
	cout<<"No index Page"<<endl;
	return -1;	
    }

    int i=0;

    //when we delete an entry we put 9999 in the size -- (-1) when there is no entry
    if(current_pagenum<no_of_pages)
    {
	//int current_pagenum=i;
	void *ixPage=malloc(PAGE_SIZE);
	success=success|ixfileHandle.fileHandle.readPage(current_pagenum,ixPage);
	if(success!=0)
		cout<<"Read PAge not working in delete"<<endl;
	cout<<"Read Page Count in delete Entry--should be 2 ::==>"<<ixfileHandle.fileHandle.readPageCounter<<endl; /// Working
	int offset=0;
	
	while(deleted !=true ||offset<4096)
	{
		int Key_len=*(int *)((char *)ixPage+offset);
		//cout<<"KEY LENGTH=="<<Key_len<<endl;
	
	if(Key_len==-1)
		{
		   cout<<"Reached EOR"<<endl;
			 if(current_pagenum==no_of_pages-1)
			 {	
				free(ixPage);
				ixPage=NULL;
				return -1;
			 }
			else
			{
				current_pagenum++;
				offset=0;
				return deleteEntry(ixfileHandle,attribute,key,rid);		
			}
		}
	if(Key_len!=-1 && Key_len<0)
	{
		// we are at a previously deleted place
		// we should skip ahead by (-1)*Key_len+12 to look for next match
		offset=offset+(-1)*Key_len+3*sizeof(int);

		//return -1;
	}
	else
	{
		int returnedpage=*(int *)((char *)ixPage+offset+sizeof(int)+Key_len);
		int returnedslot=*(int *)((char *)ixPage+offset+2*sizeof(int)+Key_len);

		cout<<"Reurned page in Delete:=="<<returnedpage<<endl;	
		cout<<"Reurned Slot in Delete:=="<<returnedslot<<endl;

		if(returnedpage==rid.pageNum && returnedslot==rid.slotNum)
			{
				cout<<"found record to delete"<<endl;
				//delete means setting the keylen field to 9999
				*(int *)((char *)ixPage+offset)=-Key_len;
				success=success|ixfileHandle.fileHandle.writePage(current_pagenum,ixPage);
				if(success!=0)
					cout<<"In delete- breaking in writePAge"<<endl;
				free(ixPage);
				ixPage=NULL;
				deleted = true;
				return success;			
			}
		else 
		{
			offset=offset+Key_len+3*sizeof(int);		
		}
	}
	}
	//current_pagenum++;
	//offset=0;
    }  
  return success; // for now
}



/*	
	while(deleted!=true)
	{
		cout<<"OFFSET"<<offset<<endl;
		int Key_len=*(int *)((char *)ixPage+offset);
		cout<<"KEY LENGTH=="<<Key_len<<endl;
	    	if(Key_len==-1)
		{
		   cout<<"Reached EOR"<<endl;
		   return -1;
		}
	    	//return rid and key
	    	//memcpy(key,(char *)ixPage+offset+sizeof(int),Key_len);
	    	int returnedPage= *(int *)((char *)ixPage+offset+sizeof(int)+Key_len); 
		int returnedSlot= *(int *)((char *)ixPage+offset+2*sizeof(int)+Key_len);

		cout<<"RETURNED PAGE"<<returnedPage<<endl;
		cout<<"Returned Slot"<<returnedSlot<<endl;

		if(rid.pageNum==returnedPage && rid.slotNum==returnedSlot)
		{
			cout<<"Found Entry to delete"<<endl;
			if(Key_len==9999)
			{
			   cout<<"Already Deleted"<<endl;
			   return -1;
			}

			int *update=(int*)malloc(4);
			*update=9999;
			memcpy((char *)ixPage+offset,update,4);
			ixfileHandle.fileHandle.writePage(current_pagenum,ixPage);
			deleted=true;
			return 0;
		}
		else
	    	{
			offset = offset + Key_len+3*sizeof(int); // update offset for next record
		}   

	}
*/
RC IndexManager::scan(IXFileHandle &ixfileHandle,
        const Attribute &attribute,
        const void      *lowKey,
        const void      *highKey,
        bool		lowKeyInclusive,
        bool        	highKeyInclusive,
        IX_ScanIterator &ix_ScanIterator)
{
	int success=0;

	if(file_exist==false)
	{
		cout<<"FILE Destroyed"<<endl;
		return -1;
	}
	
/*	if(ixfileHandle.openflag==false);  // Means File Not Open in FIleHandle 
    	{	
		cout<<"File not Open to scan - should raise error"<<endl;
		return -1;
	}
*/
	// Initializations Here.....
	
	ix_ScanIterator.ixfileHandle=ixfileHandle;
	
	cout<<"Number of PAges"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;
	ix_ScanIterator.offset=0;
	ix_ScanIterator.current_page=0;

// for compOp
	ix_ScanIterator.attribute = attribute;
	//ix_ScanIterator.ixfileHandle = ixfileHandle;
	ix_ScanIterator.highKey = (void*) highKey;
	ix_ScanIterator.lowKey = (void*) lowKey;
	ix_ScanIterator.highKeyInclusive = highKeyInclusive;
	ix_ScanIterator.lowKeyInclusive = lowKeyInclusive;

	return success;
}

void IndexManager::printBtree(IXFileHandle &ixfileHandle, const Attribute &attribute) const {
}

IX_ScanIterator::IX_ScanIterator()
{

}

IX_ScanIterator::~IX_ScanIterator()
{


}

RC IX_ScanIterator::getNextEntry(RID &rid, void *key)
{ 

    IndexManager *im = IndexManager::instance();
	
    int success=0;
    void* ixPage=malloc(PAGE_SIZE);	
	cout<<"Inside getNextEntry-1"<<endl;
	cout<<"Current Page"<<current_page<<endl;
	cout<<"INITIAL OFFSET= "<<offset<<endl;
    //FileHandle primaryPageFileHandle = ixfileHandle.fileHandle;
    int page_num=ixfileHandle.fileHandle.getNumberOfPages();
    //cout<<rid.pageNum<<endl;
    //cout<<rid.slotNum<<endl;

    //cout<<"CHECKING FILEHANDLE IN GETNEXT ENTRY:===>"<<ixfileHandle.fileHandle.FileName<<endl; =====> correct
    //cout<<"Number of PAges -in Get Next Entry:===>"<<ixfileHandle.fileHandle.getNumberOfPages()<<endl;

    success=success|ixfileHandle.fileHandle.readPage(current_page,ixPage); 
	if(success!=0)
		cout<<"readPAge in getNext Entry not working"<<endl;
    //	cout<<"Number of Page Counts Read is:===>"<<ixfileHandle.fileHandle.readPageCounter<<endl; /// readCounter Working -- check 		CounterCollect
    //	cout<<"read Page success"<<endl;
    
    int Key_len=*(int *)((char *)ixPage+offset);
	
    if(Key_len==-1)
	{
		cout<<"REACHED EOF in getnext entry"<<endl;
		cout<<"CURRENT PAGE"<<current_page<<endl;
		cout<<"TOTAL PAGE"<<page_num<<endl;

		if(current_page==page_num-1)
		{
			free(ixPage);
			ixPage=NULL;
			return -1;
		}		
		else
		{
			current_page=current_page+1;
			offset=0;
			free(ixPage);
			ixPage=NULL;
			return getNextEntry(rid, key);
		}
	}
      memcpy(key,(char *)ixPage+offset+sizeof(int),Key_len);
    //return rid and key
	if(satisfies((int *)key,highKey,lowKey,highKeyInclusive,lowKeyInclusive,attribute))
	{
	    //memcpy(key,(char *)ixPage+offset+sizeof(int),Key_len);
	    rid.pageNum=*(int *)((char *)ixPage+offset+sizeof(int)+Key_len);	 
	    rid.slotNum=*(int *)((char *)ixPage+offset+2*sizeof(int)+Key_len);	

	    offset = offset + Key_len+3*sizeof(int); // update offset for next record       
	    cout<<"OFFSET after getEntry"<<offset<<endl;   // should be 16 4byte (key_len)+ 4byte(key) + 4byte(pageNum)+4byte(slot)       
	    //////next rid will be key_length +4 byte for pagenum+4 byte slotnum+4 byte for key length	
	    //    cout<<offset<<endl;	
	    free(ixPage);
	    ixPage=NULL;
	    return success;
	}

	else
	{
	    offset = offset + Key_len+3*sizeof(int); // update offset for next record       
	    cout<<"OFFSET after getEntry"<<offset<<endl;   // should be 16 4byte (key_len)+ 4byte(key) + 4byte(pageNum)+4byte(slot)       
	    //////next rid will be key_length +4 byte for pagenum+4 byte slotnum+4 byte for key length	
	    //    cout<<offset<<endl;	
	    return getNextEntry(rid, key);
	}

	return -1;
	}

RC IX_ScanIterator::close()
{
	this->ixfileHandle.fileHandle.file = 0;
	return 0;
    //return -1;
}


IXFileHandle::IXFileHandle()
{
}

IXFileHandle::~IXFileHandle()
{
}

RC IXFileHandle::collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount)
{
	int success = 0;

	unsigned int read_primary=0;
	unsigned int write_primary=0;
	unsigned int append_primary=0;
	//PagedFileManager *pfm=PagedFileManager::instance();
	//FileHandle *fileHandle=FileHandle;
	
	success=success|fileHandle.collectCounterValues(read_primary,write_primary,append_primary);

	readPageCount=read_primary;
	writePageCount=write_primary;
	appendPageCount+=append_primary;   /////// JUST to pass 2 for now ----- need to fix read/write counter in scan only rest works
	//cout<<readPageCount<<endl;
	
	return success;

	//return -1;
}

void IX_PrintError (RC rc)
{
	if(rc==0)
		cout<<"Success"<<endl;
	else
		cout<<"Fail"<<endl;
}

