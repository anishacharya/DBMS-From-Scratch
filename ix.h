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

	// Create index file(s) to manage an index
	RC createFile(const string &fileName, const unsigned &numberOfPages);

	//// create primary pages

	RC createnewBucket(FileHandle fileHandle);

	RC createnewOverflow(FileHandle fileHandle);

	// Delete index file(s)
	RC destroyFile(const string &fileName);

	// Open an index and returns an IXFileHandle
	RC openFile(const string &fileName, IXFileHandle &ixFileHandle);

	// Close an IXFileHandle.
	RC closeFile(IXFileHandle &ixfileHandle);


	// The following functions  are using the following format for the passed key value.
	//  1) data is a concatenation of values of the attributes
	//  2) For INT and REAL: use 4 bytes to store the value;
	//     For VarChar: use 4 bytes to store the length of characters, then store the actual characters.

	// Insert an entry to the given index that is indicated by the given IXFileHandle
	RC insertEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);

	RC write_in_a_bucket(int available_space,IXFileHandle &ixfileHandle,
			const Attribute &attribute, const void *key, const RID &rid,void* buffer,int record_length,int Bucket);

	RC write_in_a_OverflowBucket(int available_space,IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key,
			const RID &rid,void *buffer,int length_of_key,int BucketNum);

	RC loopy_insert_overflow(int Bucket,IXFileHandle ixfileHandle,const Attribute &attribute, const void *key, const RID &rid);

	RC bucketnum(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);
	// Delete an entry from the given index that is indicated by the given IXFileHandle
	RC deleteEntry(IXFileHandle &ixfileHandle, const Attribute &attribute, const void *key, const RID &rid);

	// scan() returns an iterator to allow the caller to go through the results
	// one by one in the range(lowKey, highKey).
	// For the format of "lowKey" and "highKey", please see insertEntry()
	// If lowKeyInclusive (or highKeyInclusive) is true, then lowKey (or highKey)
	// should be included in the scan
	// If lowKey is null, then the range is -infinity to highKey
	// If highKey is null, then the range is lowKey to +infinity

	// Initialize and IX_ScanIterator to supports a range search
	RC scan(IXFileHandle &ixfileHandle,
			const Attribute &attribute,
			const void        *lowKey,
			const void        *highKey,
			bool        lowKeyInclusive,
			bool        highKeyInclusive,
			IX_ScanIterator &ix_ScanIterator);

	// Generate and return the hash value (unsigned) for the given key
	unsigned hash(const Attribute &attribute, const void *key);


	// Print all index entries in a primary page including associated overflow pages
	// Format should be:
	// Number of total entries in the page (+ overflow pages) : ??
	// primary Page No.??
	// # of entries : ??
	// entries: [xx] [xx] [xx] [xx] [xx] [xx]
	// overflow Page No.?? liked to [primary | overflow] page No.??
	// # of entries : ??
	// entries: [xx] [xx] [xx] [xx] [xx]
	// where [xx] shows each entry.
	RC printIndexEntriesInAPage(IXFileHandle &ixfileHandle, const Attribute &attribute, const unsigned &primaryPageNumber);

	// Get the number of primary pages
	RC getNumberOfPrimaryPages(IXFileHandle &ixfileHandle, unsigned &numberOfPrimaryPages);

	// Get the number of all pages (primary + overflow)
	RC getNumberOfAllPages(IXFileHandle &ixfileHandle, unsigned &numberOfAllPages);

	int Record_length(const Attribute &attribute, const void *data);

protected:
	IndexManager   ();                            // Constructor
	~IndexManager  ();                            // Destructor

private:
	static IndexManager *_index_manager;
};


class IX_ScanIterator {
public:
	IX_ScanIterator();  							// Constructor
	~IX_ScanIterator(); 							// Destructor

	RC getNextEntry(RID &rid, void *key);  		// Get next matching entry
	RC close();             						// Terminate index scan
};


class IXFileHandle {
public:
	// Put the current counter values of associated PF FileHandles into variables
	RC collectCounterValues(unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount);

	IXFileHandle();  							// Constructor
	~IXFileHandle(); 							// Destructor

	FileHandle fileHandle0; //Actual File
	FileHandle fileHandle1; // Metadata File
	FILE* file;

	//int util_pagenum;

private:
	unsigned readPageCounter;
	unsigned writePageCounter;
	unsigned appendPageCounter;
};

// print out the error message for a given return code
void IX_PrintError (RC rc);


#endif
