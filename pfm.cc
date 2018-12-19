#include "pfm.h"
#include<iostream>
#include<fstream>
#include<cstdio>
#include<cassert>
#include<cstdlib>
#include<istream>
#include<cmath>

using namespace std;

PagedFileManager* PagedFileManager::_pf_manager = 0;

PagedFileManager* PagedFileManager::instance()
{
    if(!_pf_manager)
        _pf_manager = new PagedFileManager();

    return _pf_manager;
}
PagedFileManager::PagedFileManager()
{
}

PagedFileManager::~PagedFileManager()
{
_pf_manager=NULL;
}
RC PagedFileManager::createFile(const char *fileName)
{
	FILE* file=fopen(fileName,"r+b");					// open the file in read mode. returns NULL
													// means does not exist
	if (file!=NULL)
	{
	perror("The following error occurred");
		return -1;
	}
	else
	{
				FILE* file=fopen(fileName,"w+b");	//create the file named fileName
				//fclose(file);						//close the file
				return 0;
	}
}
RC PagedFileManager::destroyFile(const char *fileName)
{
	//FILE* file =fopen(fileName,"r");				//checks if File exists
	if(fopen(fileName,"r+b")==NULL)
	{
		perror("The Following error occurred");
		// cout<<"Error-No Files to Destroy"<<endl;
		return -1;
	}
	else
	{
		remove(fileName);							// removes File named fileName
		return 0;
	}
}
RC PagedFileManager::openFile(const char *fileName, FileHandle &fileHandle)
{
	//FILE* file;
	//cout<<"in open file"<<endl;

	fileHandle.FileName = (char*)fileName;
	if(fileHandle.flag==true)			//flag is set means fileHandle is
										//Handle of some other open File
	{
		perror("The following Error Occurred");  //check if it is handle of some other object
		return -1;
	}
	else
	{

		fileHandle.file=fopen(fileName,"r+b");
		if(fileHandle.file!=NULL)   //check whether the file exist or not
		{
			fileHandle.flag=true;					//for this particular filehandle flag is set.

			//move the position indicator to the end
			fseek (fileHandle.file, 0, SEEK_END);
			//Ref: http://www.cplusplus.com/reference/cstdio/fseek/
			int pageSize = ftell(fileHandle.file);    // get bytes
			fileHandle.no_of_pages= ceil(pageSize/PAGE_SIZE);
			// gets number of pages ceil because if 1.5 we should return 2
			//fileHandle.name="fileName"; //set the name as fileName
			return 0;
		}
		return -1;
	}
}
RC PagedFileManager::closeFile(FileHandle &fileHandle)
{
	//cout<<"in write file"<<endl;
	if(fileHandle.flag!=true)   					   //// means the file is not open
		{
			perror("The following Error Occurred");  //check if it is handle of some other object
			return -1;

		}
	else
	{
		fclose(fileHandle.file);
		fileHandle.flag=false;
		return 0;
	}
}
FileHandle::FileHandle()
{
	flag=false;    ////initially flag set as false
	no_of_pages=0;
    readPageCounter=0;
    writePageCounter=0;
    appendPageCounter=0;
}
FileHandle::~FileHandle()
{
}
RC FileHandle::readPage(PageNum pageNum, void *data)
{	//cout<<"in read"<<endl;
	if (pageNum >= getNumberOfPages())
			return -1;
	else
	{
		if(file==NULL)
		{
			cout<<"no file open";
			return -1;
		}
		else
		{
			fseek(file,(pageNum)*(PAGE_SIZE),SEEK_SET); //because pagNum starts from 0 thus pagenum+1
			//long lSize = ftell (file);
			//size_t result=
			fread(data,1,PAGE_SIZE,file);
			//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );

			readPageCounter++;
			//Ref:http://www.cplusplus.com/reference/cstdio/fread/
			//if (result != PAGE_SIZE) {fputs ("Reading error",stderr); return -1;}
			//rewind (file);
			return 0;
			//fclose(file);
			//free(data);
		}
	}
return -1;
}
RC FileHandle::writePage(PageNum pageNum, const void *data)
{	//cout<<"in write"<<endl;
	if(pageNum>=getNumberOfPages())
		return -1;
	else
	{
		if(file==NULL)
				{
					cout<<"error- no file open"<<endl;
					//perror("no file open");
					return -1;
				}
		else
		{
			fseek(file, (pageNum)*(PAGE_SIZE), SEEK_SET);
			fwrite(data,PAGE_SIZE,1,file);
			rewind(file);
			writePageCounter++;
			//fflush(file);
			//fclose(file);
			return 0;
		}
	}
}
/*
RC FileHandle::writePage(PageNum pageNum, const void *data)
{
if(pageNum>=getNumberOfPages())
return -1;
if(fseek(file,pageNum*PAGE_SIZE,SEEK_SET)!=0)
return -1;
if(fwrite(data,PAGE_SIZE,1,file)!=1)
return -1;
return 0;
}
*/
RC FileHandle::appendPage(const void *data)
{	//cout<<"in append"<<endl;
	fseek(file, 0, SEEK_END);         ////moves the pointer to the end of the page
	fwrite(data, 1, PAGE_SIZE, file); ////writes data in a chunk= page size i.e. a new page added

	appendPageCounter++;
	no_of_pages++; 					   //// increment page count of the file.
	return 0;

}
unsigned FileHandle::getNumberOfPages()
{
	return no_of_pages;
}


////*****TO DO*******////////
RC FileHandle::collectCounterValues(unsigned &readPageCount,
		unsigned &writePageCount, unsigned &appendPageCount)
{

	readPageCount=readPageCounter;
	writePageCount=writePageCounter;
	appendPageCount=appendPageCounter;
	return 0;
}
/*
unsigned FileHandle::getNumberOfPages()
{
    //return no_of_pages;
	int static NumberOfPages=0;
	long l,m,n;

	l=file.tellg();//marks the start of the file
	file.seekg(0,std::ios::end);
	m=file.tellg();//marks the end of the file
	n=l-m;//size of file
	NumberOfPages=n/PAGE_SIZE;

	return NumberofPages;
}
*/


