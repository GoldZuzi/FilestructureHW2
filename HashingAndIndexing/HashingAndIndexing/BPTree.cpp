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
	node *elseNode; //entry 내의 모든 key값보다 큰 key 값들이 저장된 노드의 포인터
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
	
	/*leftNode는 기존의 노드를 그대로 사용
	rightNode는 새로 생성하여 leftNode의
	split 지점 이후의 원소들을 복사함*/
	leftNode = nodeToSplit;	//
	rightNode = createNode(nodeToSplit->isLeaf, nodeToSplit);	
	//copyNodeEntry(leftNode, nodeToSplit, 0, 0, splitFactor + 1);
	copyNodeEntry(rightNode, nodeToSplit, 0, splitFactor + 1, ENTRYSIZE - splitFactor + 1);
	
	/*rightNode로 복사된 원소들을 leftNode에서 제거함*/
	leftNode->elseNode = leftNode->nodeEntry[splitFactor + 1].a.childNode;
	for (int i = splitFactor + 1; i < ENTRYSIZE; i++) {
		free(&leftNode->nodeEntry[i]);
	}

	leftNode->entryCount = splitFactor + 1;
	rightNode->entryCount = ENTRYSIZE - splitFactor + 1;
	
	/*ParentNode를 지정하는 과정*/
	if (nodeToSplit->isRoot) {   //분할되는 노드가 기존의 루트노드였을 경우
		parentNode = createNode(false, NULL); //ParentNode 생성
		root = parentNode;
		parentNode->isRoot = true;
		leftNode->isRoot = false;
	}
	
	else { //ParentNode는 원래의 노드가 가리키던 노드
		parentNode = nodeToSplit->parentNode;
		
		/*parentNode가 꽉 찼을 경우 스플릿*/
		if (parentNode->entryCount == ENTRYSIZE) {
			parentNode = split(parentNode, splitKey);
		}
	}

	leftNode->parentNode = parentNode;
	rightNode->parentNode = parentNode;
	leftNode->nextLeafNode = rightNode;
	
	/*---------여기에 parent노드에 삽입되는 부분 작성--------*/
	internalNodeInsert(parentNode, splitKey, leftNode, rightNode);

	if (key < splitKey) return leftNode;
	else return rightNode;
}

void internalNodeInsert(node *internalNode, float key, node *leftNode, node *rightNode) {
	entry newEntry;
	blockPointer ptr;
	
	/*삽입될 엔트리가 가리키는 포인터는 왼쪽 child 노드*/
	ptr.childNode = leftNode;
	/*엔트리 생성*/
	createNewEntry(&newEntry, internalNode->isLeaf, key, ptr);
	int index = internalNode->entryCount;
	for (int i = 0; i < internalNode->entryCount; i++) {
		if (key <= internalNode->nodeEntry[i].score) {
			index = i;
			break;
		}
	}
	/*엔트리 삽입*/
	for (int i = internalNode->entryCount; i > index; i--) {
		memcpy(&internalNode->nodeEntry[i], &internalNode->nodeEntry[i - 1], sizeof(struct entry)); //엔트리를 하나씩 right shift
	}
	memset(&internalNode->nodeEntry[index], 0, sizeof(struct entry)); //삽입할 엔트리 번지 초기화
	memcpy(&internalNode->nodeEntry[index], &newEntry, sizeof(struct entry)); //엔트리 삽입

	/*엔트리가 노드의 가장 끝에 삽입될 경우*/
	if (index == internalNode->entryCount) {
		internalNode->elseNode = rightNode;
	}
	/*엔트리가 노드의 중간 어딘가 삽입될 경우*/
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

	/*key값이 이미 트리에 존재하는지 체크*/
	if (search(key) >= 0) {
		return false;
	}

	/*엔트리를 삽입할 리프노드를 찾는 단계*/
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

	/*해당 리프가 꽉 찼을 경우 노드 분할*/
	if (tmpNode->entryCount == ENTRYSIZE) {
		tmpNode = split(tmpNode, key);
	}

	/*새로운 엔트리 생성*/
	createNewEntry(&newEntry, tmpNode->isLeaf, key, ptr);
	
	/*노드에 삽입할 위치 지정*/
	index = tmpNode->entryCount;
	for (int i = 0; i < tmpNode->entryCount; i++) {
		if (key <= tmpNode->nodeEntry[i].score) {
			index = i;
			break;
		}
	}
	/*노드에 엔트리를 삽입*/
	if (tmpNode->isLeaf) { 
		/*노드가 리프일 경우(무조건 리프일듯)*/		
		for (int i = tmpNode->entryCount; i > index; i--) {
			memcpy(&tmpNode->nodeEntry[i], &tmpNode->nodeEntry[i - 1], sizeof(struct entry)); //엔트리를 하나씩 right shift
		}
		memset(&tmpNode->nodeEntry[index], 0, sizeof(struct entry)); //삽입할 엔트리 번지 초기화
		memcpy(&tmpNode->nodeEntry[index], &newEntry, sizeof(struct entry)); //엔트리 삽입
		tmpNode->entryCount++;
	}
	return true;
}



