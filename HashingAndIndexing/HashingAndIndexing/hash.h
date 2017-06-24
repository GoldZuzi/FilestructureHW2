#pragma once
#define MAXNUM 128

struct student {
	char name[20];
	unsigned int studentID;
	float score;
	unsigned int advisorID;
};

struct professor {
	char name[20];
	int professorID;
	int salary;
};

union dataSet {
	student studentSet[MAXNUM];
	professor professorSet[146];
};

struct bucket {
	int bucketNum;
	int count;
	dataSet data;
};

struct hashNode
{
	int prefix;
	char key[33];
	bucket *buc;
	hashNode* next;
};



struct hashTable {
	int prefix;
	int blockNum[5000];
};

void createHashTableFile(hashTable *tb);
void insert(student st, hashNode *node);
void split(hashNode* oldNode);
bool hashCompare(char *val, char* key, int prefix);
hashNode *createHashNode();
void initNode(hashNode *node);
bucket *createBucket();
student *createStudent();
void createDB(hashNode *node);
char *uintToBinary(unsigned int i);
bool hashCompare(char *val, char* key, int prefix);
void makeHashTable(hashTable *tb, hashNode *hsNode);
extern const bool studentTable;
extern const bool profTable;
extern bool tableType;