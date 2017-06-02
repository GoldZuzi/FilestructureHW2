#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include "hash.h"
using namespace std;


int maxPrefix = 0;
int bucketCount = 0;
int nodeCount = 0;



void initBucket(bucket *bk) {
	bk->count = 0;
	bk->prefix = 0;
	bk->bucketNum = 0;
}
void initNode(hashNode *node) {
	node->prefix = 0;
	for (int i = 0; i < 32; i++) {
		node->key[i] = '0';
	}
	node->buc = createBucket();
	initBucket(node->buc);
	node->expand = false;
	node->next = NULL;
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

student *createStudent() {
	student *st;
	st = (struct student *)malloc(sizeof(struct student));
	memset(st, 0, sizeof(struct bucket));
	return st;
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
//
//hashNode* search(unsigned int id) {
//	
//}

bool hashCompare(char *val, char* key, int prefix) {
	for (int i = 0 ; i < prefix; i++) {
		if (key[31 - i] != val[31 - i])
			return false;
	}
	return true;
}
//int calculateBlockNum(char *key) {
//	int dec = 0;
//	for (int i = maxPrefix; i < 32; i++) {
//
//	}
//
//}

void split(hashNode* oldNode) {
	char *tmp;
	bool isEqual;
	int k = MAXNUM;
	hashNode *newNode;
	newNode = createHashNode();
	newNode->buc = createBucket();
	bucketCount++;
	newNode->buc->bucketNum = bucketCount;
	strcpy(newNode->key, oldNode->key);
	oldNode->key[31 - oldNode->prefix] = '0';
	newNode->key[31 - oldNode->prefix] = '1';
	
	oldNode->buc->prefix++;
	oldNode->prefix++;
	newNode->buc->prefix++;
	newNode->prefix++;
	if (newNode->prefix > maxPrefix) maxPrefix = newNode->prefix;

	/*분할된 버킷사이의 옮기는 작업*/
	for (int i = 0; i < k; i++) {
		tmp = uintToBinary(oldNode->buc->studentSet[i].studentID);
		isEqual = hashCompare(tmp, oldNode->key, oldNode->prefix);
		if (!isEqual) {
			newNode->buc->studentSet[newNode->buc->count] = oldNode->buc->studentSet[i];
			newNode->buc->count++;
			memmove(&oldNode->buc->studentSet[i], &oldNode->buc->studentSet[i + 1], sizeof(struct student) * (oldNode->buc->count - i - 1));
			oldNode->buc->count--;
			k--;
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
void createHashTableFile(hashNode *node) {
	FILE* pFile = NULL;
	fopen_s(&pFile, "student.hash", "wb");
	for (int i = 0; i < nodeCount; i++) {
		fwrite(&node->prefix, sizeof(int), 1, pFile);
		fwrite(node->key, sizeof(char), 33, pFile);
		fwrite(&i, sizeof(int), 1, pFile);
		node = node->next;
	}
	fclose(pFile);
}

void createDB(hashNode *node) {
	FILE *pFile = NULL;
	fopen_s(&pFile, "students.DB", "wb");
	for (int i = 0; i < nodeCount; i++) {
		fwrite(node->buc->studentSet, sizeof(struct student), MAXNUM, pFile);
		node = node->next;
	}
	fclose(pFile);
}







