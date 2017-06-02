#pragma once
#define MAXNUM 128

struct student {
	char name[20];
	unsigned int studentID;
	float score;
	unsigned int advisorID;
};

struct bucket {
	int bucketNum;
	int count;
	int prefix;
	student studentSet[MAXNUM];
};

struct hashNode
{
	int prefix;
	char key[33];
	bucket *buc;
	bool expand;
	hashNode* next;
};

//struct hashTable {
//	int prefix;
//	vector<hashNode*> nodeSet;
//
//};



void createHashTableFile(hashNode *node);
void insert(student st, hashNode *node);
void split(hashNode* oldNode);
bool hashCompare(char *val, char* key, int prefix);
//hashNode* search(unsigned int id);
hashNode *createHashNode();
void initNode(hashNode *node);
bucket *createBucket();
student *createStudent();
void createDB(hashNode *node);