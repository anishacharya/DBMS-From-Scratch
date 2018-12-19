#ifndef _ixtest_util_h_
#define _ixtest_util_h_

#ifndef _success_
#define _success_
const int success = 0;
#endif

#ifndef _fail_
#define _fail_
const int fail = -1;
#endif

#include "ix.h"

void assertCreateIndexFile(RC expected, IndexManager * indexManager, const string &indexFileName);

void assertDestroyIndexFile(RC expected,IndexManager * indexManager, const string &indexFileName);

void assertOpenIndexFile(RC expected, 
        IndexManager * indexManager, 
        const string &indexFileName, 
        IXFileHandle &ixfileHandle);

void assertCloseIndexFile(RC expected,
        IndexManager * indexManager,
        IXFileHandle &ixfileHandle);

void assertInsertEntry(RC expected,
        IndexManager * indexManager, 
        IXFileHandle & ixfileHandle, 
        const Attribute &attribute, 
        const void * key, 
        const RID &rid);

void assertDeleteEntry(RC expected, 
        IndexManager * indexManager, 
        IXFileHandle & ixfileHandle, 
        const Attribute &attribute, 
        const void * key, 
        const RID &rid);

void assertInitalizeScan(RC expected,
        IndexManager * indexManager, 
        IXFileHandle &ixfileHandle,
        const Attribute &attribute,
        const void *lowKey,
        const void *highKey,
        bool lowKeyInclusive, 
        bool highKeyInclusive,
        IX_ScanIterator &ix_ScanIterator);

void assertCollectCounter(RC expected, 
        IXFileHandle & ixfileHandle, 
        unsigned &readPageCount, 
        unsigned &writePageCount, 
        unsigned &appendPageCount);

void assertCloseIterator(RC expected, IX_ScanIterator &iter);
#endif


