#include "qe.h"
#include "../ix/ix.h"
#include "../rbf/rbfm.h"
#include <cstring>
#include <string>
#include <bitset>


Filter::Filter(Iterator* input, const Condition &condition) 
{
	this->iter = input;
	this->cond = condition;
}

int attrLen(const void *data,int start, const vector <Attribute> &recordDescriptor,int attr)
{
	int fields=recordDescriptor.size();
	int null_bytes=ceil(double(fields)/8);

	//unsigned char *nullsIndicator = (unsigned char *) malloc(null_bytes);
	//memcpy(nullsIndicator,(char *)data + start, null_bytes);

	unsigned end=0;


	//int end=0;
	//int shifterbits = (null_bytes*8) - 1;
	//bool nullBit = false;


	cout<<"RD-Size=" <<recordDescriptor.size()<<endl;

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

RC getAttrFromRecord(const void *data0,const void *buffer,const vector<Attribute> &recordDescriptor,const string attributeName)
{
	int attr=0;
	for (attr=0;attr<recordDescriptor.size();attr++)
	{
		if(recordDescriptor[attr].name==attributeName)	
			break;
	}
	cout<<"attr: ="<<attr<<endl;	//attr num 
	if(attr==recordDescriptor.size())
			return -1;
	
	//cout<<"ATTR NUM"<<attr<<endl;
	int length=attrLen(buffer,0,recordDescriptor,attr); // this basically takes pointer to the attr to be read
	cout<<"Length="<<length<<endl;
	// so upto this attr length
	//int fields=recordDescriptor.size();
	//int null_bytes=ceil(double(fields)/8);
	//unsigned char *nullsIndicator = (unsigned char *) malloc(null_bytes);
	//memcpy(nullsIndicator,(char *)data + start, null_bytes);
	//unsigned offset=null_bytes;

	
	if(recordDescriptor[attr].type==TypeInt)
	{
		cout<<"recordDescriptor[attr].type=INT"<<endl;
		cout<<"Actual Data"<<(int*)((char *)buffer+length)<<endl;
		memcpy((char *)data0,(char *)buffer+length,sizeof(int));
		return sizeof(int);
	}
	else if(recordDescriptor[attr].type==TypeReal)
	{
		cout<<"Actual Data"<<(int*)((char *)buffer+length)<<endl;
		cout<<"recordDescriptor[attr].type=real"<<endl;
		memcpy((char *)data0,(char *)buffer+length,sizeof(float));
		return sizeof(float);
	}
	else if(recordDescriptor[attr].type==TypeVarChar)
	{
		cout<<"recordDescriptor[attr].type=varchar"<<endl;
		cout<<"Actual Data"<<(int*)((char *)buffer+length)<<endl;
		memcpy((char *)data0,(char *)buffer+length,4);
		memcpy((char *)data0+sizeof(int),(char *)buffer+length+sizeof(float),*(int *)data0);
		return *((int *)data0)+sizeof(int);			
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
		int offset=0;        	
		int *buffer=(int*)malloc(PAGE_SIZE);

		this->iter->getNextTuple(buffer);

		for (int j=1;j<9;j++)
		{
			cout<<"j="<<j<<endl;
			cout<<(buffer[j])<<endl;
		 }

		//cout<<"initial actual data"<<*(float *)((char *)buffer)<<endl;
		// now buffer should have the data
		// in buffer first we will have null bytes then the attributes actual data
		//memcpy()				
		//if(getNextTuple(buffer) == QE_EOF)
		//{
		//	return QE_EOF;
		//}
//		cout<<"now actual data"<<(int *)((char *)buffer)<<endl;
		//std::cout << "actual data = " << std::bitset<32>(buffer)  << std::endl;
		//std::cout << "actual data = " << std::bitset<32>(buffer+4)  << std::endl;
        	vector <Attribute> recordDescriptor;
		//cout<<"recordDescriptor.size();"<<recordDescriptor.size()<<endl;
        	iter->getAttributes(recordDescriptor);
//works OK

		cout<<"recordDescriptor[attr].name="<<recordDescriptor[0].name<<endl;
		cout<<"recordDescriptor[attr].type="<<recordDescriptor[0].type<<endl;
		cout<<"recordDescriptor[attr].name="<<recordDescriptor[1].name<<endl;
		cout<<"recordDescriptor[attr].type="<<recordDescriptor[1].type<<endl;
		cout<<"recordDescriptor[attr].name="<<recordDescriptor[2].name<<endl;
		cout<<"recordDescriptor[attr].type="<<recordDescriptor[2].type<<endl;			//TESTED OK
		//cout<<"recordDescriptor[attr].name="<<recordDescriptor[3].name<<endl;
		
		// so, all attributes are in the vector now
		int fields=recordDescriptor.size();
		int null_bytes=ceil(double(fields)/8);
        	//int offset=null_bytes;

		cout<<"Null bytes: ="<<null_bytes<<endl;
		// so these bytes are NULL info in the input data 

		// copy that in output // update offset
		offset=null_bytes;
		//memcpy((int *)((char *)data),buffer,offset);

		//int offset=0;
		//int *null = (int *) malloc(1);
		//null=0;
		
		//memcpy((int *)((char *)data),null,1);
		memset ( (int *)((char *)data), 0, 1);	 // set first byte NUll	
		// now not failing in Null check - so first byte is set to zero 
//		cout<<"error here"<<endl;


//		memcpy((char *)data+1,(char *)buffer+sizeof(int)+4,sizeof(float));
//		memcpy((char *)data+1+sizeof(float),(char *)buffer+sizeof(int)+4+sizeof(float),sizeof(int));

        	for(int i=0; i < attrNames.size(); i++)
        	{
			cout<<"Processing attr="<<attrNames[i]<<endl;
			int returnval=getAttrFromRecord((void *)((char *)data+offset),(void *)((char *)buffer),recordDescriptor,attrNames[i]);
			cout<<"return"<<returnval<<endl;        		
			offset=offset+returnval;
			cout<<"offset"<<offset<<endl;        					
        	}

        	free(buffer);
		buffer=NULL;
		//free(data);
		//data=NULL;
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





// ... the rest of your implementations go here
