#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
using namespace std;

//struct leafNode;
//struct internalNode;
struct entry;
struct node;

node *root;

#define ENTRYSIZE 6
#define Fanout ENTRYSIZE + 1;

typedef union blockPointer {
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
	bool isLeaf;
	bool isRoot;
	int entryCount;
	entry nodeEntry[ENTRYSIZE];
	node *elseNode; //entry ���� ��� key������ ū key ������ ����� ����� ������
	node *nextLeafNode;
	//node *parentNode;
};

node* createNode(bool isLeaf, node *parent) {
	node *newNode;
	newNode = (struct node *)malloc(sizeof(struct node));
	memset(newNode, 0, sizeof(struct node));
	newNode->isLeaf = isLeaf;
	newNode->parentNode = parent;
	return newNode;
}

void initNode(node *initialNode) {
	initialNode->isLeaf = true;
	initialNode->isRoot = true;
	initialNode->parentNode = NULL;
	initialNode->nextLeafNode = NULL;
	initialNode->entryCount = 0;
}

void copyNodeEntry(node *dst, node *src, int dstFirst, int srcFirst, int size) {
	size = size * sizeof(struct entry);
	memcpy(&dst->nodeEntry[dstFirst], &src->nodeEntry[srcFirst], size);
}

static struct node* split(node *nodeToSplit, float key) {
	int splitFactor = (ENTRYSIZE >> 1) - 1;
	float splitKey = nodeToSplit->nodeEntry[splitFactor].score;
	
	node *parentCheck = NULL;
	node *parentNode = NULL;
	node *leftNode = NULL;
	node *rightNode = NULL;
	node *oldNode = nodeToSplit;
	
	/*leftNode�� ������ ��带 �״�� ���
	rightNode�� ���� �����Ͽ� leftNode��
	split ���� ������ ���ҵ��� ������*/
	leftNode = nodeToSplit;	//
	rightNode = createNode(nodeToSplit->isLeaf, nodeToSplit);	
	//copyNodeEntry(leftNode, nodeToSplit, 0, 0, splitFactor + 1);
	copyNodeEntry(rightNode, nodeToSplit, 0, splitFactor + 1, ENTRYSIZE - splitFactor + 1);
	
	/*rightNode�� ����� ���ҵ��� leftNode���� ������*/
	leftNode->elseNode = leftNode->nodeEntry[splitFactor + 1].a.childNode;
	for (int i = splitFactor + 1; i < ENTRYSIZE; i++) {
		free(&leftNode->nodeEntry[i]);
	}

	leftNode->entryCount = splitFactor + 1;
	rightNode->entryCount = ENTRYSIZE - splitFactor + 1;
	
	/*ParentNode�� �����ϴ� ����*/
	if (nodeToSplit->isRoot) {   //���ҵǴ� ��尡 ������ ��Ʈ��忴�� ���
		parentNode = createNode(false, NULL); //ParentNode ����
		root = parentNode;
		parentNode->isRoot = true;
		leftNode->isRoot = false;
	}
	
	else { //ParentNode�� ������ ��尡 ����Ű�� ���
		parentNode = nodeToSplit->parentNode;
		
		/*parentNode�� �� á�� ��� ���ø�*/
		if (parentNode->entryCount == ENTRYSIZE) {
			parentNode = split(parentNode, splitKey);
		}
	}

	leftNode->parentNode = parentNode;
	rightNode->parentNode = parentNode;
	leftNode->nextLeafNode = rightNode;
	
	/*---------���⿡ parent��忡 ���ԵǴ� �κ� �ۼ�--------*/
	internalNodeInsert(parentNode, splitKey, leftNode, rightNode);

	if (key < splitKey) return leftNode;
	else return rightNode;
}

void internalNodeInsert(node *internalNode, float key, node *leftNode, node *rightNode) {
	entry newEntry;
	blockPointer ptr;
	
	/*���Ե� ��Ʈ���� ����Ű�� �����ʹ� ���� child ���*/
	ptr.childNode = leftNode;
	/*��Ʈ�� ����*/
	createNewEntry(&newEntry, internalNode->isLeaf, key, ptr);
	int index = internalNode->entryCount;
	for (int i = 0; i < internalNode->entryCount; i++) {
		if (key <= internalNode->nodeEntry[i].score) {
			index = i;
			break;
		}
	}
	/*��Ʈ�� ����*/
	for (int i = internalNode->entryCount; i > index; i--) {
		memcpy(&internalNode->nodeEntry[i], &internalNode->nodeEntry[i - 1], sizeof(struct entry)); //��Ʈ���� �ϳ��� right shift
	}
	memset(&internalNode->nodeEntry[index], 0, sizeof(struct entry)); //������ ��Ʈ�� ���� �ʱ�ȭ
	memcpy(&internalNode->nodeEntry[index], &newEntry, sizeof(struct entry)); //��Ʈ�� ����

	/*��Ʈ���� ����� ���� ���� ���Ե� ���*/
	if (index == internalNode->entryCount) {
		internalNode->elseNode = rightNode;
	}
	/*��Ʈ���� ����� �߰� ��� ���Ե� ���*/
	else {
		internalNode->nodeEntry[index + 1].a.childNode = rightNode;
	}
	internalNode->entryCount++;
}

int search(float key) {
	return searchDetail(root, key);
}

int searchDetail(node *currentNode, float key)
{
	int count = 0;
	while (1) {
		if (currentNode->isLeaf) break;
		if (currentNode->entryCount > 0) {
			for (int i = 0; i < currentNode->entryCount; i++) {
				count++;
				if (key <= currentNode->nodeEntry[i].score) {
					currentNode = currentNode->nodeEntry[i].a.childNode;
					break;
				}
				if (count == currentNode->entryCount)
					currentNode = currentNode->elseNode;
			}
			count = 0;
		}
		else
		{
			return -1;
		}
	}
	for (int i = 0; i < currentNode->entryCount; i++) {
		if (key == currentNode->nodeEntry[i].score)
			return currentNode->nodeEntry[i].a.blockNum;
	}
	return -1;
}

void createNewEntry(entry *newEntry, bool isLeaf, float key, blockPointer ptr) {
	newEntry->score = key;
	if (isLeaf) 
		newEntry->a.blockNum = ptr.blockNum;
	else
		newEntry->a.childNode = ptr.childNode;
}

bool insert(float key, blockPointer ptr) {
	entry newEntry;
	node *tmpNode = root;
	int count;
	int index;

	/*key���� �̹� Ʈ���� �����ϴ��� üũ*/
	if (search(key) >= 0) {
		return false;
	}

	/*��Ʈ���� ������ ������带 ã�� �ܰ�*/
	while (1) {
		if (tmpNode->isLeaf) break;
		if (tmpNode->entryCount > 0) {
			for (int i = 0; i < tmpNode->entryCount; i++) {
				count++;
				if (key <= tmpNode->nodeEntry[i].score) {
					tmpNode = tmpNode->nodeEntry[i].a.childNode;
					break;
				}
				if (count == tmpNode->entryCount)
					tmpNode = tmpNode->elseNode;
			}
			count = 0;
		}
	}

	/*�ش� ������ �� á�� ��� ��� ����*/
	if (tmpNode->entryCount == ENTRYSIZE) {
		tmpNode = split(tmpNode, key);
	}

	/*���ο� ��Ʈ�� ����*/
	createNewEntry(&newEntry, tmpNode->isLeaf, key, ptr);
	
	/*��忡 ������ ��ġ ����*/
	index = tmpNode->entryCount;
	for (int i = 0; i < tmpNode->entryCount; i++) {
		if (key <= tmpNode->nodeEntry[i].score) {
			index = i;
			break;
		}
	}
	/*��忡 ��Ʈ���� ����*/
	if (tmpNode->isLeaf) { 
		/*��尡 ������ ���(������ �����ϵ�)*/		
		for (int i = tmpNode->entryCount; i > index; i--) {
			memcpy(&tmpNode->nodeEntry[i], &tmpNode->nodeEntry[i - 1], sizeof(struct entry)); //��Ʈ���� �ϳ��� right shift
		}
		memset(&tmpNode->nodeEntry[index], 0, sizeof(struct entry)); //������ ��Ʈ�� ���� �ʱ�ȭ
		memcpy(&tmpNode->nodeEntry[index], &newEntry, sizeof(struct entry)); //��Ʈ�� ����
		tmpNode->entryCount++;
	}
	return true;
}



