#ifndef _ix_h_
#define _ix_h_

#include <vector>
#include <string>

#include "../rbf/rbfm.h"

# define IX_EOF (-1)  // end of the index scan

class IX_ScanIterator;
class IXFileHandle;

class IndexManager {

    public:
        static IndexManager* instance();
	
	bool file_exist=false;

        // Create an index file
        RC createFile(const string &fileName);

        // Delete an index file
        RC destroyFile(const string &fileName);

        // Open an index and return a file handle
        RC openFile(const string &fileName, IXFileHandle &ixFileHandle);

        // Close a file handle for an index. 
        RC closeFile(IXFileHandle &ixfileHandle);

        // Insert an entry into the given index that is indicated by the given ixfileHandle
        RC insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);
	 RC beyondRootInsert(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid,int i);
        // Delete an entry from the given index that is indicated by the given fileHandle
        RC deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);

        // Initialize and IX_ScanIterator to supports a range search
        RC scan(IXFileHandle &ixfileHandle,
                const Attribute &attribute,
                const void *lowKey,
                const void *highKey,
                bool lowKeyInclusive,
                bool highKeyInclusive,
                IX_ScanIterator &ix_ScanIterator);

        // Print the B+ tree JSON record in pre-order
        void printBtree(IXFileHandle &ixfileHandle, const Attribute &attribute) const;

	int current_pagenum=0;

    protected:
        IndexManager();
        ~IndexManager();

    private:
        static IndexManager *_index_manager;
	PagedFileManager *pfm=PagedFileManager::instance();
};


class IXFileHandle {
    public:
        // Put the current counter values of associated PF FileHandles into variables
	PagedFileManager *pfm=PagedFileManager::instance();
        RC collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount);

        IXFileHandle();  							// Constructor
        ~IXFileHandle(); 							// Destructor
	

	void setFileHandle(FileHandle& FileHandle) {
		this->fileHandle = FileHandle;
	}
	
	FileHandle fileHandle;							
	
	//PagedFileManager Filehandle;

	bool openflag=true;
};

class IX_ScanIterator {
    public:
        IX_ScanIterator();  							// Constructor
        ~IX_ScanIterator(); 							// Destructor
	//FileHandle fileHandle;	
	IXFileHandle ixfileHandle;
	//IXFileHandle ixFileHandle;

	Attribute attribute;
	int pg_to_read;
	void * next_value;
	RID next_rid;
	//FileHandle fileHandle;
	void * lowKey,* highKey;
	bool lowKeyInclusive,highKeyInclusive;

	int offset;
	int current_page;


        RC getNextEntry(RID &rid, void *key);  		// Get next matching entry
        RC close();             						// Terminate index scan
};



// print out the error message for a given return code
void IX_PrintError (RC rc);

#endif

