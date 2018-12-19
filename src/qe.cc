#include "qe.h"
#include "../ix/ix.h"
#include "../rbf/rbfm.h"
#include <cstring>
#include <string>

int attrLen(const void *data,int start, const vector <Attribute> &recordDescriptor,int attr)
{
	//int fields=recordDescriptor.size();
	//int null_bytes=ceil(double(fields)/8);
	//unsigned char *nullsIndicator = (unsigned char *) malloc(null_bytes);
	//memcpy(nullsIndicator,(char *)data + start, null_bytes);
	//unsigned end=null_bytes;
	int end=0;

	//int shifterbits = (null_bytes*8) - 1;
	//bool nullBit = false;
	for(unsigned i=0;i<recordDescriptor.size() && i<attr;i++)
	{
		//nullBit = nullsIndicator[0] & (1 << shifterbits);
		//if(!nullBit) 
	//{
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
	//}
	//	shifterbits--;
	}
	return end;
}

RC getAttrFromRecord(const void *data,const void *buffer,const vector<Attribute> &recordDescriptor,const string attributeName)
{
	int attr=0;
	for (attr=0;attr<recordDescriptor.size();attr++)
	{
		if(recordDescriptor[attr].name==attributeName)	
			break;
	}
	cout<<"attr: ="<<attr<<endl;
	if(attr==recordDescriptor.size())
			return -1;
	
	//cout<<"ATTR NUM"<<attr<<endl;
	int length=attrLen(buffer,0,recordDescriptor,attr);
	cout<<"Length="<<length<<endl;
	// so upto this attr length
	
	if(recordDescriptor[attr].type==TypeInt)
	{
		cout<<"recordDescriptor[attr].type=INT"<<endl;
		cout<<"Actual Data"<<((char *)buffer+length)<<endl;
		memcpy((char *)data,(char *)buffer+length,4);
		return sizeof(int);
	}
	else if(recordDescriptor[attr].type==TypeReal)
	{
		cout<<"Actual Data"<<(char *)buffer+length<<endl;
		memcpy((char *)data,(char *)buffer+length,4);
		return sizeof(float);
	}
	else if(recordDescriptor[attr].type==TypeVarChar)
	{
		cout<<"Actual Data"<<(char *)buffer+length<<endl;
		memcpy((char *)data,(char *)buffer+length,4);
		memcpy((char *)data+sizeof(int),(char *)buffer+length+4,*(int *)data);
		return *((int *)data)+sizeof(int);			
	}
	return -1;
}

Project::Project(Iterator *input,                    // Iterator of input R
              const vector<string> &attrNames)
{
	this->iter = input;
	this->attrNames = attrNames;
}

RC Project::getNextTuple(void *data)
{	
		
	int  current_bit;
        void *buffer=malloc(PAGE_SIZE);

        this->iter->getNextTuple(buffer);
        vector <Attribute> recordDescriptor;
        iter->getAttributes(recordDescriptor);
        
        int fields=recordDescriptor.size();
        int null_bytes=ceil(double(fields)/8);
        
	unsigned char *nullbytesAttr = (unsigned char *) malloc(null_bytes);  //These are the nullbytes for our outgoing attributes 
        //(which might be less than our incoming tuple)
        unsigned char *nullbytesRecv = (unsigned char *) malloc(null_bytes);  //These are the nullbytes for our recieved tuple
        memset(nullbytesAttr, 0, null_bytes);  //Set to zero 
        
        memcpy(nullbytesRecv,(char *)buffer, null_bytes);  //Copy in to recieved from buffer
        int shifterbits=(null_bytes*8)-1;
        int offset=0;

		/*
		We need to loop over all the records in the descripter, because we only know the null bit offset for all records
		Not just our subset of attr names
		*/
		bool nullBit = false;
		for(int j=0; j < recordDescriptor.size(); j++ ) {
			nullBit = nullbytesRecv[0] & (1 << shifterbits);
	        for(int i=0; i < attrNames.size(); i++)  {
	        	if(attrNames[i] == recordDescriptor[j].name) {
		        	if(nullBit) {
		        		//Set the current_bit in the attr null bytes to 1, we got a null
		        		cout << "Null Encountered" << endl;
						current_bit = pow(2.0,((null_bytes*8)-i)-1);
						nullbytesAttr[0] = nullbytesAttr[0] | current_bit;
		        	} else {
		        		cout << "No Null" << endl;
		        		int returnval=getAttrFromRecord((void *)((char *)data+offset+null_bytes),(void *)((char *)buffer),recordDescriptor,attrNames[i]);
			            offset=offset+returnval;
		        	}
	        	}
	        }
	        shifterbits--;
		}
        
		memcpy((char *)data,nullbytesAttr,null_bytes);
        free(buffer);
        free(nullbytesAttr);
        free(nullbytesRecv);
        buffer=NULL;
        nullbytesAttr=NULL;
        nullbytesRecv=NULL;
        return 0;
}

template <class T>
bool Satisfy(T x,T y,CompOp compOp)
{
	if(compOp==NO_OP)
		return true;
	if(compOp==EQ_OP)
		return x==y;
	if(compOp==NE_OP)
		return x!=y;
	if(compOp==LT_OP)
		return x<y;
	if(compOp==GT_OP)
		return x>y;
	if(compOp==LE_OP)
		return x<=y;
	if(compOp==GE_OP)
		return x>=y;

	return false;
}
bool satisfiesCompOp(CompOp compOp,void *value,void *attr_value,int attr_type)
{
	if(compOp==NO_OP)
	return true;
	if(attr_type==0)
		return Satisfy(*(int *)attr_value,*(int *)value,compOp);
	if(attr_type==1)
		return Satisfy(*(int *)attr_value,*(int *)value,compOp);
	if(attr_type==2)
	{
		string a="",b="";
		for(int i=0;i<*(int *)attr_value;i++)
		{
			a+=*((char *)attr_value+sizeof(int));
		}
		for(int i=0;i<*(int *)value;i++)
		{
			b+=*((char *)value+sizeof(int));
		}
		return Satisfy <string>(a,b,compOp);
	}
}


bool Filter::CheckSatisfyCond(void *data,Condition cond,Iterator *in)
{
	vector <Attribute> rd;
	in->getAttributes(rd);

	int indL=0,indR=0;

	for(indL=0;indL<rd.size();indL++)
	{
		if(rd[indL].name==cond.lhsAttr)
		cout<<"match attr LHS"<<endl;
			break;
	}

	for(indR=0;indR<rd.size();indR++)
	{
		if(rd[indR].name==cond.rhsAttr)
		cout<<"match attr RHS"<<endl;
			break;
	}

	//if(indL>=rd.size()||(cond.bRhsIsAttr&&indR>=rd.size())||rd[indL].type!=rd[indR].type)
		//return false;

	void *lhs=malloc(PAGE_SIZE);
	void *rhs=malloc(PAGE_SIZE);
	//cout<<indL<<"\n";
	getAttrFromRecord(lhs,data,rd,cond.lhsAttr);
	if(cond.bRhsIsAttr)
	getAttrFromRecord(rhs,data,rd,cond.rhsAttr);
	else
	{
		int l=0;
		if(rd[indL].type==0)
			l=sizeof(int);
		if(rd[indL].type==1)
			l=sizeof(float);
		if(rd[indL].type==2)
			l=*(int *)cond.rhsValue.data;
		rhs=memcpy(rhs,cond.rhsValue.data,l);
	}
	cout<<(*(int *)lhs)<<" "<<(*(int *)rhs)<<"\n";
	bool res= satisfiesCompOp(cond.op,rhs,lhs,rd[indL].type);
	free(lhs);
	free(rhs);
	return res;
}

Filter::Filter(Iterator* input, const Condition &condition) 
{
	iter=input;
	vector <Attribute> rd;
	iter->getAttributes(rd);
	//cout<<rd[1].name<<"\n";
	cond=condition;
}




// ... the rest of your implementations go here
