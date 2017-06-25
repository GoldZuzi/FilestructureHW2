#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#define ENTRYSIZE 511

typedef struct node;

union blockPointer {
	unsigned int blockNum;
	unsigned int childNode;
};

struct entry
{
	float score;
	blockPointer a;
};

struct node
{
	int entryCount;
	entry nodeEntry[ENTRYSIZE];
	int elseNode; //entry 내의 모든 key값보다 큰 key 값들이 저장된 노드의 포인터
};

extern node* root;
extern FILE* pFile;
extern int depth;
extern int btNodeCount;
extern int btRootNodeNum;
extern const bool studentTable;
extern const bool profTable;
extern bool tableType;

node* createNode();
void initNode(node *initialNode);
int search(float key);
bool insert(float key, blockPointer ptr);
void internalNodeInsert(node *internalNode, float key, int leftNode, int rightNode);
void createNewEntry(entry *newEntry, bool isLeaf, float key, blockPointer ptr);
int searchDetail(node *currentNode, float key);
void writeNodeInfo(node *currentNode, int nodeBlockNum);
void makeFile();
void loadFile();
void loadNodeInfo(node *currentNode, int nodeBlockNum);
//node* sequencialSearch(int k);
//node* seqSearchDetail(node *currentNode, int k, int currentDepth);
void writeCommonInfo();
int rangeSearch(float min, float max);
void studentExactSearch(int key);
void profExactSearch(int key);

