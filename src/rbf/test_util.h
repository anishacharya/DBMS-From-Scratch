#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <sys/stat.h>
#include <stdlib.h> 
#include <string.h>
#include <stdexcept>
#include <stdio.h> 
#include <math.h>

#include "pfm.h"
#include "rbfm.h"

using namespace std;

const int success = 0;


// Check whether the given file exists
bool FileExists(string &fileName)
{
    struct stat stFileInfo;

    if(stat(fileName.c_str(), &stFileInfo) == 0) return true;
    else return false;
}

// Get the given file's size
ifstream::pos_type getFileSize(string fileName)
{
    std::ifstream in(fileName.c_str(), std::ifstream::in | std::ifstream::binary);
    in.seekg(0, std::ifstream::end);
    cout << fileName << " - file size:" << in.tellg() << endl;
    return in.tellg();
}


// Compare the sizes of two files
bool compareFileSizes(string fileName1, string fileName2) {
	streampos s1, s2;
	ifstream in1(fileName1.c_str(), ifstream::in | ifstream::binary);
	in1.seekg(0, ifstream::end);
	s1 = in1.tellg();

	ifstream in2(fileName2.c_str(), ifstream::in | ifstream::binary);
	in2.seekg(0, ifstream::end);
	s2 = in2.tellg();

	cout << "File 1 size: " << s1 << endl;
	cout << "File 2 size: " << s2 << endl;

	if (s1 != s2) {
		return false;
	}
	return true;
}

// After createFile() check
int createFileShouldSucceed(string &fileName) 
{
    if(FileExists(fileName))
    {
        return 0;
    }
    else
    {
        cout << "[Fail] Failed to create the file: " << fileName << endl;
        cout << "[Fail] Test Case Failed!" << endl << endl;
        return -1;
    }
}

// After destroyFile() check
int destroyFileShouldSucceed(string &fileName) 
{
    if(FileExists(fileName))
    {
        cout << "[Fail] Failed to destory the file: " << fileName << endl;
        cout << "[Fail] Test Case Failed!" << endl << endl;
        return -1;
    }
    else
    {
        return 0;
    }
}

// Calculate actual bytes for nulls-indicator based on the given field counts
// 8 fields = 1 byte
int getActualByteForNullsIndicator(int fieldCount) {

	return ceil((double) fieldCount / CHAR_BIT);
}


// Record Descriptor for TweetMessage
void createRecordDescriptorForTweetMessage(
		vector<Attribute> &recordDescriptor) {

	Attribute attr;
	attr.name = "tweetid";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "userid";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "sender_location";
	attr.type = TypeReal;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "send_time";
	attr.type = TypeReal;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "referred_topics";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 500;
	recordDescriptor.push_back(attr);

	attr.name = "message_text";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 500;
	recordDescriptor.push_back(attr);

}

// Function to prepare the data in the correct form to be inserted/read
void prepareRecordForTweetMessage(int fieldCount, unsigned char *nullFieldsIndicator, const int tweetid, const int userid,
		const float sender_location, const float send_time,
		const int referred_topicsLength, const string &referred_topics,
		const int message_textLength, const string &message_text, void *buffer,
		int *recordSize) {

	int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicator for the fields
    memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a record consists of four fields and they are all nulls, then the bit representation will be: [11110000]

	// Is the tweetid field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 7);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &tweetid, sizeof(int));
		offset += sizeof(int);
	}

	// Is the userid field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 6);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &userid, sizeof(int));
		offset += sizeof(int);
	}

	// Is the sender_location field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 5);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &sender_location, sizeof(float));
		offset += sizeof(float);
	}

	// Is the send_time field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 4);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &send_time, sizeof(float));
		offset += sizeof(float);
	}


	// Is the referred_topics field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 3);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &referred_topicsLength, sizeof(int));
		offset += sizeof(int);
		memcpy((char *) buffer + offset, referred_topics.c_str(),referred_topicsLength);
		offset += referred_topicsLength;
	}

	// Is the message_text field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 2);

	if (!nullBit) {
		memcpy((char *) buffer + offset, &message_textLength, sizeof(int));
		offset += sizeof(int);
		memcpy((char *) buffer + offset, message_text.c_str(), message_textLength);
		offset += message_textLength;
	}

	*recordSize = offset;

}


void prepareLargeRecordForTweetMessage(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer,
		int *size) {
	int offset = 0;

	int count = index % 400 + 1;
	int text = index % 26 + 65;
	string suffix (count, text);

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// tweetid = index
	int tweetid = index + 1;

	memcpy((char *) buffer + offset, &tweetid, sizeof(int));
	offset += sizeof(int);

	// userid
	memcpy((char *) buffer + offset, &index, sizeof(int));
	offset += sizeof(int);

	// sender_location
	float sender_location = (float) count + 0.1;

	memcpy((char *) buffer + offset, &sender_location, sizeof(float));
	offset += sizeof(float);

	// send_time
	float send_time = (float) index + 0.2;

	memcpy((char *) buffer + offset, &send_time, sizeof(float));
	offset += sizeof(float);

	// referred_topics
	string referred_topics = "shortcut_menu" + suffix;
	int referred_topicsLength = referred_topics.length();

	memcpy((char *) buffer + offset, &referred_topicsLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, referred_topics.c_str(),
			referred_topicsLength);
	offset += referred_topicsLength;

	// message_text
	string message_text = "shortcut-menu is helpful: " + suffix;
	int message_textLength = message_text.length();
	memcpy((char *) buffer + offset, &message_textLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, message_text.c_str(), message_textLength);
	offset += message_textLength;

	*size = offset;
}


// Record Descriptor for TwitterUser
void createRecordDescriptorForTwitterUser(vector<Attribute> &recordDescriptor) {

	Attribute attr;
	attr.name = "userid";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "screen_name";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 300;
	recordDescriptor.push_back(attr);

	attr.name = "lang";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 300;
	recordDescriptor.push_back(attr);

	attr.name = "friends_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "statuses_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "username";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 300;
	recordDescriptor.push_back(attr);

	attr.name = "followers_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "satisfaction_score";
	attr.type = TypeReal;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

}

// Record Descriptor for TwitterUser
void createRecordDescriptorForTwitterUser2(vector<Attribute> &recordDescriptor) {

	Attribute attr;
	attr.name = "userid";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "screen_name";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 800;
	recordDescriptor.push_back(attr);

	attr.name = "lang";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 800;
	recordDescriptor.push_back(attr);

	attr.name = "friends_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "statuses_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "username";
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 800;
	recordDescriptor.push_back(attr);

	attr.name = "followers_count";
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

	attr.name = "satisfaction_score";
	attr.type = TypeReal;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);

}

void prepareLargeRecordForTwitterUser(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer, int *size) {
	int offset = 0;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	int count = index % 200 + 1;
	int text = index % 26 + 97;
	string suffix(count, text);

	// userid = index
	memcpy((char *) buffer + offset, &index, sizeof(int));
	offset += sizeof(int);

	// screen_name
	string screen_name = "NilaMilliron@" + suffix;
	int screen_nameLength = screen_name.length();

	memcpy((char *) buffer + offset, &screen_nameLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, screen_name.c_str(), screen_nameLength);
	offset += screen_nameLength;

	// lang
	string lang = "En " + suffix;
	int langLength = lang.length();

	memcpy((char *) buffer + offset, &langLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, lang.c_str(), langLength);
	offset += langLength;

	// friends_count
	int friends_count = count - 1;

	memcpy((char *) buffer + offset, &friends_count, sizeof(int));
	offset += sizeof(int);

	// statuses_count
	int statuses_count = count + 1;

	memcpy((char *) buffer + offset, &statuses_count, sizeof(int));
	offset += sizeof(int);

	// username
	string username = "Nilla Milliron " + suffix;
	int usernameLength = username.length();

	memcpy((char *) buffer + offset, &usernameLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, username.c_str(), usernameLength);
	offset += usernameLength;

	// followers_count
	int followers_count = count + 2;

	memcpy((char *) buffer + offset, &followers_count, sizeof(int));
	offset += sizeof(int);

	// satisfaction_score
	float satisfaction_score = (float) count + 0.1;

	memcpy((char *) buffer + offset, &satisfaction_score, sizeof(float));
	offset += sizeof(float);

	*size = offset;
}

// Function to prepare the data in the correct form to be inserted/read
void prepareRecordForTwitterUser(int fieldCount, unsigned char *nullFieldsIndicator, const int userid, const int screen_nameLength,
		const string &screen_name, const int langLength, const string &lang,
		const int friends_count, const int statuses_count,
		const int usernameLength, const string &username,
		const int followers_count, const float satisfaction_score, void *buffer,
		int *recordSize) {

	int offset = 0;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// userid
	memcpy((char *) buffer + offset, &userid, sizeof(int));
	offset += sizeof(int);

	// screen_name
	memcpy((char *) buffer + offset, &screen_nameLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, screen_name.c_str(), screen_nameLength);
	offset += screen_nameLength;

	// lang
	memcpy((char *) buffer + offset, &langLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, lang.c_str(), langLength);
	offset += langLength;

	// friends_count
	memcpy((char *) buffer + offset, &friends_count, sizeof(int));
	offset += sizeof(int);

	// statuses_count
	memcpy((char *) buffer + offset, &statuses_count, sizeof(int));
	offset += sizeof(int);

	// username
	memcpy((char *) buffer + offset, &usernameLength, sizeof(int));
	offset += sizeof(int);
	memcpy((char *) buffer + offset, username.c_str(), usernameLength);
	offset += usernameLength;

	// followers_count
	memcpy((char *) buffer + offset, &followers_count, sizeof(int));
	offset += sizeof(int);

	// satisfaction_score
	memcpy((char *) buffer + offset, &satisfaction_score, sizeof(float));
	offset += sizeof(float);

	*recordSize = offset;
}


void createRecordDescriptor(vector<Attribute> &recordDescriptor) {

    Attribute attr;
    attr.name = "EmpName";
    attr.type = TypeVarChar;
    attr.length = (AttrLength)30;
    recordDescriptor.push_back(attr);

    attr.name = "Age";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    recordDescriptor.push_back(attr);

    attr.name = "Height";
    attr.type = TypeReal;
    attr.length = (AttrLength)4;
    recordDescriptor.push_back(attr);

    attr.name = "Salary";
    attr.type = TypeInt;
    attr.length = (AttrLength)4;
    recordDescriptor.push_back(attr);

}

// Function to prepare the data in the correct form to be inserted/read
void prepareRecord(int fieldCount, unsigned char *nullFieldsIndicator, const int nameLength, const string &name, const int age, const float height, const int salary, void *buffer, int *recordSize)
{
    int offset = 0;

	// Null-indicators
    bool nullBit = false;
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicator for the fields
    memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// Beginning of the actual data    
	// Note that the left-most bit represents the first field. Thus, the offset is 7 from right, not 0.
	// e.g., if a record consists of four fields and they are all nulls, then the bit representation will be: [11110000]

	// Is the name field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 7);

	if (!nullBit) {
		memcpy((char *)buffer + offset, &nameLength, sizeof(int));
	    offset += sizeof(int);
	    memcpy((char *)buffer + offset, name.c_str(), nameLength);
	    offset += nameLength;
	}

	// Is the age field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 6);
	if (!nullBit) {
	    memcpy((char *)buffer + offset, &age, sizeof(int));
	    offset += sizeof(int);
	}

	// Is the height field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 5);
	if (!nullBit) {
	    memcpy((char *)buffer + offset, &height, sizeof(float));
	    offset += sizeof(float);
	}

	// Is the height field not-NULL?
	nullBit = nullFieldsIndicator[0] & (1 << 4);
	if (!nullBit) {
	    memcpy((char *)buffer + offset, &salary, sizeof(int));
	    offset += sizeof(int);
	}

    *recordSize = offset;
}


void createLargeRecordDescriptor(vector<Attribute> &recordDescriptor)
{
    int index = 0;
    char *suffix = (char *)malloc(10);
    for(int i = 0; i < 10; i++)
    {
        Attribute attr;
        sprintf(suffix, "%d", index);
        attr.name = "attr";
        attr.name += suffix;
        attr.type = TypeVarChar;
        attr.length = (AttrLength)50;
        recordDescriptor.push_back(attr);
        index++;

        sprintf(suffix, "%d", index);
        attr.name = "attr";
        attr.name += suffix;
        attr.type = TypeInt;
        attr.length = (AttrLength)4;
        recordDescriptor.push_back(attr);
        index++;

        sprintf(suffix, "%d", index);
        attr.name = "attr";
        attr.name += suffix;
        attr.type = TypeReal;
        attr.length = (AttrLength)4;
        recordDescriptor.push_back(attr);
        index++;
    }
    free(suffix);
}


void prepareLargeRecord(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer, int *size)
{
    int offset = 0;

    // compute the count
    int count = index % 50 + 1;

    // compute the letter
    char text = index % 26 + 97;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// Actual data
    for(int i = 0; i < 10; i++)
    {
        memcpy((char *)buffer + offset, &count, sizeof(int));
        offset += sizeof(int);

        for(int j = 0; j < count; j++)
        {
            memcpy((char *)buffer + offset, &text, 1);
            offset += 1;
        }

        // compute the integer
        memcpy((char *)buffer + offset, &index, sizeof(int));
        offset += sizeof(int);

        // compute the floating number
        float real = (float)(index + 1);
        memcpy((char *)buffer + offset, &real, sizeof(float));
        offset += sizeof(float);
    }
    *size = offset;
}


void createLargeRecordDescriptor2(vector<Attribute> &recordDescriptor) {
	int index = 0;
	char *suffix = (char *) malloc(10);
	for (int i = 0; i < 10; i++) {
		Attribute attr;
		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeInt;
		attr.length = (AttrLength) 4;
		recordDescriptor.push_back(attr);
		index++;

		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeReal;
		attr.length = (AttrLength) 4;
		recordDescriptor.push_back(attr);
		index++;

		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeVarChar;
		attr.length = (AttrLength) 60;
		recordDescriptor.push_back(attr);
		index++;

	}
	free(suffix);
}


void prepareLargeRecord2(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer, int *size) {
	int offset = 0;

	// compute the count
	int count = index % 60 + 1;

	// compute the letter
	char text = index % 26 + 65;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	for (int i = 0; i < 10; i++) {
		// compute the integer
		memcpy((char *) buffer + offset, &index, sizeof(int));
		offset += sizeof(int);

		// compute the floating number
		float real = (float) (index + 1);
		memcpy((char *) buffer + offset, &real, sizeof(float));
		offset += sizeof(float);

		// compute the varchar field
		memcpy((char *) buffer + offset, &count, sizeof(int));
		offset += sizeof(int);

		for (int j = 0; j < count; j++) {
			memcpy((char *) buffer + offset, &text, 1);
			offset += 1;
		}

	}
	*size = offset;
}


void createLargeRecordDescriptor3(vector<Attribute> &recordDescriptor) {
	int index = 0;
	char *suffix = (char *) malloc(10);
	for (int i = 0; i < 6; i++) {
		Attribute attr;
		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeVarChar;
		attr.length = (AttrLength) 60;
		recordDescriptor.push_back(attr);
		index++;

		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeInt;
		attr.length = (AttrLength) 4;
		recordDescriptor.push_back(attr);
		index++;

		sprintf(suffix, "%d", index);
		attr.name = "attr";
		attr.name += suffix;
		attr.type = TypeReal;
		attr.length = (AttrLength) 4;
		recordDescriptor.push_back(attr);
		index++;

	}
	free(suffix);
}


void prepareLargeRecord3(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer, int *size) {
	int offset = 0;

	// compute the count
	int count = index % 60 + 1;

	// compute the letter
	char text = index % 26 + 65;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	int attr_pos = 0;
	int attr_pos_in_nth_byte = 0;
	int attr_pos_in_nth_bit_in_a_byte = 0;
	
	bool nullBit = false;
	
	for (int i = 0; i < 6; i++) {
	
		attr_pos++;
		
		attr_pos_in_nth_byte = floor((double) attr_pos / CHAR_BIT);
		attr_pos_in_nth_bit_in_a_byte = CHAR_BIT - attr_pos % CHAR_BIT;
		
		nullBit = nullFieldsIndicator[attr_pos_in_nth_byte] & (1 << attr_pos_in_nth_bit_in_a_byte);
		
		if (!nullBit) {
			// compute the Varchar field
			memcpy((char *) buffer + offset, &count, sizeof(int));
			offset += sizeof(int);

			for (int j = 0; j < count; j++) {
				memcpy((char *) buffer + offset, &text, 1);
				offset += 1;
			}
		}
		
		attr_pos++;

		attr_pos_in_nth_byte = floor((double) attr_pos / CHAR_BIT);
		attr_pos_in_nth_bit_in_a_byte = CHAR_BIT - attr_pos % CHAR_BIT;
		
		nullBit = nullFieldsIndicator[attr_pos_in_nth_byte] & (1 << attr_pos_in_nth_bit_in_a_byte);

		if (!nullBit) {
			// compute the integer
			memcpy((char *) buffer + offset, &index, sizeof(int));
			offset += sizeof(int);
		}

		attr_pos++;

		attr_pos_in_nth_byte = floor((double) attr_pos / CHAR_BIT);
		attr_pos_in_nth_bit_in_a_byte = CHAR_BIT - attr_pos % CHAR_BIT;
		
		nullBit = nullFieldsIndicator[attr_pos_in_nth_byte] & (1 << attr_pos_in_nth_bit_in_a_byte);

		if (!nullBit) {
			// compute the floating number
			float real = (float) (index + 1);
			memcpy((char *) buffer + offset, &real, sizeof(float));
			offset += sizeof(float);
		}


	}
	*size = offset;
}


void createLargeRecordDescriptor4(vector<Attribute> &recordDescriptor) {
	int index = 0;
	char *suffix = (char *) malloc(10);
	Attribute attr;
	sprintf(suffix, "%d", index);
	attr.name = "attr";
	attr.name += suffix;
	attr.type = TypeVarChar;
	attr.length = (AttrLength) 2100;
	recordDescriptor.push_back(attr);
	index++;

	sprintf(suffix, "%d", index);
	attr.name = "attr";
	attr.name += suffix;
	attr.type = TypeInt;
	attr.length = (AttrLength) 4;
	recordDescriptor.push_back(attr);
	index++;

	free(suffix);
}


void prepareLargeRecord4(int fieldCount, unsigned char *nullFieldsIndicator, const int index, void *buffer, int *size) {
	int offset = 0;

	// compute the count
	int count = index % 2100 + 1;

	// compute the letter
	char text = index % 26 + 65;

	// Null-indicators
    int nullFieldsIndicatorActualSize = getActualByteForNullsIndicator(fieldCount);

	// Null-indicators
	memcpy((char *)buffer + offset, nullFieldsIndicator, nullFieldsIndicatorActualSize);
	offset += nullFieldsIndicatorActualSize;

	// compute the varchar field
	memcpy((char *) buffer + offset, &count, sizeof(int));
	offset += sizeof(int);

	for (int j = 0; j < count; j++) {
		memcpy((char *) buffer + offset, &text, 1);
		offset += 1;
	}

	// compute the integer
	memcpy((char *) buffer + offset, &index, sizeof(int));
	offset += sizeof(int);

	*size = offset;
}




