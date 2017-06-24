#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include "hash.h"
#include <math.h>
using namespace std;

int maxPrefix = 0;
int bucketCount = 0;
int nodeCount = 0;

void initBucket(bucket *bk) {
	bk->count = 0;
	bk->bucketNum = 0;
}
void initNode(hashNode *node) {
	node->prefix = 0;
	for (int i = 0; i < 32; i++) {
		node->key[i] = '0';
	}
	node->buc = createBucket();
	initBucket(node->buc);
	node->next = NULL;
	maxPrefix = 0;
	bucketCount = 0;
	nodeCount = 0;
}

char *uintToBinary(unsigned int i) {
	static char s[32 + 1] = { '0', };
	int count = 32;

	do {
		s[--count] = '0' + (char)(i & 1);
		i = i >> 1;
	} while (count);

	return s;
}


bucket *createBucket() {
	bucketCount++;
	bucket *bk;
	bk = (struct bucket *)malloc(sizeof(struct bucket));
	memset(bk, 0, sizeof(struct bucket));
	initBucket(bk);
	return bk;
}

hashNode *createHashNode() {
	nodeCount++;
	hashNode* node;
	node = (struct hashNode *)malloc(sizeof(struct hashNode));
	memset(node, 0, sizeof(struct hashNode));
	return node;
}

bool hashCompare(char *val, char* key, int prefix) {
	for (int i = 0 ; i < prefix; i++) {
		if (key[31 - i] != val[31 - i])
			return false;
	}
	return true;
}

void split(hashNode* oldNode) {
	char *tmp;
	bool isEqual;
	int k = 128;
	hashNode *newNode;
	newNode = createHashNode();
	newNode->buc = createBucket();
	newNode->buc->bucketNum = bucketCount;
	strcpy(newNode->key, oldNode->key);
	oldNode->key[31 - oldNode->prefix] = '0';
	newNode->key[31 - oldNode->prefix] = '1';
	
	//oldNode->buc->prefix++;
	oldNode->prefix++;
	//newNode->buc->prefix = oldNode->prefix;
	newNode->prefix = oldNode->prefix;
	if (newNode->prefix > maxPrefix) maxPrefix = newNode->prefix;

	/*분할된 버킷사이의 옮기는 작업*/
	for (int i = 0; i < k; i++) {
		if(tableType == studentTable)
			tmp = uintToBinary(oldNode->buc->data.studentSet[i].studentID);
		else
			tmp = uintToBinary()
		isEqual = hashCompare(tmp, oldNode->key, oldNode->prefix);
		if (!isEqual) {
			newNode->buc->studentSet[newNode->buc->count] = oldNode->buc->studentSet[i];
			newNode->buc->count++;
			memmove(&oldNode->buc->studentSet[i], &oldNode->buc->studentSet[i + 1], sizeof(struct student) * (oldNode->buc->count - i - 1));
			oldNode->buc->count--;
			k--;
			i--;
		}
		//free(tmp);
	}
	newNode->next = oldNode->next;
	oldNode->next = newNode;
}

void insert(student st, hashNode *node) {
	char *key;
	bool isEqual;
	for (int i = 0; i < nodeCount; i++) {
		key = uintToBinary(st.studentID);
		isEqual = hashCompare(key, node->key, node->prefix);
		if (isEqual) {
			break;			
		}
		else {
			node = node->next;
		}
	}
	if (node->buc->count < MAXNUM)
	{
		node->buc->studentSet[node->buc->count] = st;
		node->buc->count++;
	}
	else {
		split(node);
		key = uintToBinary(st.studentID);
		isEqual = hashCompare(key, node->key, node->prefix);
		if (isEqual) {
			node->buc->studentSet[node->buc->count] = st;
			node->buc->count++;
		}
		else {
			node = node->next;
			node->buc->studentSet[node->buc->count] = st;
			node->buc->count++;
		}
	}
}
void createHashTableFile(hashTable *tb) {
	FILE* pFile = NULL;
	fopen_s(&pFile, "student.hash", "wb");

	fwrite(tb, sizeof(hashTable), 1, pFile);

	
	fclose(pFile);
}

void makeHashTable(hashTable *tb, hashNode *hsNode) {
	//int maxPrefix = 0;
	char *binary;
	tb->prefix = maxPrefix;
	for (int i = 0; i < nodeCount; i++) {
		for (int j = 0; j < (int)pow(2, maxPrefix); j++) {
			binary = uintToBinary(j);
			if(hashCompare(binary, hsNode->key, hsNode->prefix))
				tb->blockNum[j] = i;
		}
		hsNode = hsNode->next;
	}
}

void createDB(hashNode *node) {
	FILE *pFile = NULL;
	fopen_s(&pFile, "students.DB", "wb");
	for (int i = 0; i < nodeCount; i++) {
		fwrite(node->buc->studentSet, sizeof(struct student), MAXNUM, pFile);
		free(node->buc);
		node = node->next;
	}
	fclose(pFile);
}





