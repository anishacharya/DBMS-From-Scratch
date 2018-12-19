#include<iostream>
#include<string>
#include<cstdio>
#include<map>

#ifndef _pfm_h_
#define _pfm_h_

typedef int RC;
typedef unsigned PageNum;

#define PAGE_SIZE 4096

using namespace std;

class FileHandle;

class PagedFileManager
{
public:
    static PagedFileManager* instance();                     // Access to the _pf_manager instance
    //bool is_file_exist(const char *fileName);
    //This method creates a paged file called fileName. The file should not already exist.
    RC createFile    (const char *fileName);

    RC destroyFile   (const char *fileName);                         // Destroy a file
    RC openFile      (const char *fileName, FileHandle &fileHandle); // Open a file
    RC closeFile     (FileHandle &fileHandle);                       // Close a file

protected:
    PagedFileManager();                                   // Constructor
    ~PagedFileManager();                                  // Destructor

private:
    static PagedFileManager *_pf_manager;
};


class FileHandle
{
    unsigned readPageCounter;
    unsigned writePageCounter;
    unsigned appendPageCounter;
public:
    FileHandle();                                                    // Default constructor
    ~FileHandle();                                                   // Destructor

    FILE* file;
    char *FileName;
    bool flag;

    unsigned no_of_pages;

    //string name;
    //unsigned pagenumber;

    RC readPage(PageNum pageNum, void *data);                           // Get a specific page
    RC writePage(PageNum pageNum, const void *data);                    // Write a specific page
    RC appendPage(const void *data);                                    // Append a specific page
    unsigned getNumberOfPages();                                        // Get the number of pages in the file
    RC collectCounterValues(unsigned &readPageCount,
    		unsigned &writePageCount, unsigned &appendPageCount);
};

 #endif

