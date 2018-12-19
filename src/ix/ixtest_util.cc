/*
 * =====================================================================================
 *
 *       Filename:  ixtest_util.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2015 11:23:07
 *       Revision:  none
 *       Compiler:  gcc
 *
 *   Organization:  ISG CS222
 *
 * =====================================================================================
 */

#include <cstdio>
#include <cassert>
#include <iostream>
#include "ixtest_util.h"

using namespace std;

void assertRC(RC expected, RC real){
    if (expected == success){
        cerr << "Succeeded as expected, correct!" << endl;
        assert(real == success);
    } else {
        cerr << "Failed as expected, correct!" << endl;
        assert(real != success);
    }
}

void assertCreateIndexFile(RC expected, IndexManager * indexManager, const string &indexFileName){
    RC rc = indexManager->createFile(indexFileName);   
    if (rc == success)
    {
        cerr << "Index File:" << indexFileName << " Created Successfully" << endl;
    } else {
        cerr << "Index File:" << indexFileName << " could not be created. Failed" << endl;
    }
    assertRC(expected, rc);
}

void assertDestroyIndexFile(RC expected, IndexManager * indexManager, const string &indexFileName){
    RC rc = indexManager->destroyFile(indexFileName);

    if (rc == success){
        cerr << "Index File:" << indexFileName << " Destroyed Successfully" << endl;
    } else {
        cerr << "Index File:" << indexFileName << " could not be destroyed. Failed" << endl;
    }
    assertRC(expected, rc);
}

void assertOpenIndexFile(RC expected, IndexManager * indexManager, const string &indexFileName, 
        IXFileHandle &ixfileHandle){
    RC rc = indexManager->openFile(indexFileName, ixfileHandle);
    if(rc == success)
    {
        cerr << "Index File, " << indexFileName << " Opened Successfully" << endl;
    }
    else
    {
        cerr << "Index File, " << indexFileName << " could not be opened. Failed" << endl;
    }
    assertRC(expected, rc);
}

void assertCloseIndexFile(RC expected, IndexManager * indexManager, IXFileHandle &ixfileHandle){
    RC rc = indexManager->closeFile(ixfileHandle);
    if(rc == success)
    {
        cerr << "Index File Closed Successfully" << endl;
    }
    else
    {
        cerr << "Index File could not be closed. Failed" << endl;
    }
    assertRC(expected, rc);

}

void assertInsertEntry(RC expected,IndexManager * indexManager, IXFileHandle & ixfileHandle, 
        const Attribute &attribute,  const void * key, const RID &rid){
    RC rc = indexManager->insertEntry(ixfileHandle, attribute, key, rid);
    if (rc != success){
        cerr << "Failed Inserting Entry..." << endl;
    }
    assertRC(expected, rc);
}

void assertDeleteEntry(RC expected, IndexManager * indexManager, IXFileHandle & ixfileHandle, 
        const Attribute &attribute, const void * key, const RID &rid){
    RC rc = indexManager->deleteEntry(ixfileHandle, attribute, key, rid);
    if (rc != success){
        cerr << "Failed deleting Entry..." << endl;
    }
    assertRC(expected, rc);
}

void assertInitalizeScan(RC expected, IndexManager * indexManager, IXFileHandle &ixfileHandle,
        const Attribute &attribute,const void *lowKey,const void *highKey,
        bool lowKeyInclusive, bool highKeyInclusive,
        IX_ScanIterator &ix_ScanIterator){
    RC rc = indexManager->scan(ixfileHandle, attribute, lowKey, highKey, lowKeyInclusive, highKeyInclusive, 
            ix_ScanIterator);
    if(rc == success)
    {
        cerr << "Scan Opened Successfully!" << endl;
    }
    else
    {
        cerr << "Failed Opening Scan!" << endl;
    }
    assertRC(expected, rc);
}

void assertCollectCounter(RC expected, IXFileHandle & ixfileHandle, 
        unsigned &readPageCount, unsigned &writePageCount, unsigned &appendPageCount){
    RC rc = ixfileHandle.collectCounterValues(readPageCount, writePageCount, appendPageCount);
    if(rc != success)
    {
        cerr << "collectCounterValues() failed." << endl;
    }
    assertRC(expected, rc);
}

void assertCloseIterator(RC expected, IX_ScanIterator &ix_ScanIterator){
    RC rc = ix_ScanIterator.close();
    if(rc == success)
    {
        cerr << "Scan Closed Successfully!" << endl;
    }
    else
    {
        cerr << "Failed Closing Scan..." << endl;
    }
    assertRC(expected, rc);
}

