#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
using namespace std;

#define ENTRYSIZE 2

typedef struct node;

union blockPointer {
	int blockNum;
	node *childNode;
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
	node *elseNode; //entry 내의 모든 key값보다 큰 key 값들이 저장된 노드의 포인터
};
extern node* root;

node* createNode();
void initNode(node *initialNode);
int search(float key);
bool insert(float key, blockPointer ptr);
void internalNodeInsert(node *internalNode, float key, node *leftNode, node *rightNode);
void createNewEntry(entry *newEntry, bool isLeaf, float key, blockPointer ptr);
int searchDetail(node *currentNode, float key);

