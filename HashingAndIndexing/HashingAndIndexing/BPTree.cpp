#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include "BPTree.h"
using namespace std;
int depth;

node* createNode() {
	node *newNode;
	newNode = (struct node *)malloc(sizeof(struct node));
	memset(newNode, 0, sizeof(struct node));
	//newNode->isLeaf = isLeaf;
	//newNode->parentNode = parent;
	return newNode;
}

void initNode(node *initialNode) {
	//initialNode->isLeaf = true;
	initialNode->elseNode = NULL;
	initialNode->entryCount = 0;
	depth = 0;
}

void copyNodeEntry(node *dst, node *src, int dstFirst, int srcFirst, int size) {
	size = size * sizeof(struct entry);
	memcpy(&dst->nodeEntry[dstFirst], &src->nodeEntry[srcFirst], size);
}

static struct node* split(node *nodeToSplit, float key, vector<node*> &parentSet, int parentIndex, bool isLeaf) {
	int splitFactor = ((ENTRYSIZE -1) >> 1);
	float splitKey = nodeToSplit->nodeEntry[splitFactor].score;
	
	node *parentNode = NULL;
	node *leftNode = NULL;
	node *rightNode = NULL;
	
	/*leftNode는 기존의 노드를 그대로 사용
	rightNode는 새로 생성하여 leftNode의
	split 지점 이후의 원소들을 복사함*/
	leftNode = nodeToSplit;	//
	//rightNode = createNode(nodeToSplit->isLeaf);	
	rightNode = createNode();
	copyNodeEntry(rightNode, nodeToSplit, 0, splitFactor + 1, ENTRYSIZE - splitFactor + 1);
		
	if (isLeaf) {
		rightNode->elseNode = leftNode->elseNode;
		leftNode->elseNode = rightNode;
	}
	else leftNode->elseNode = leftNode->nodeEntry[splitFactor + 1].a.childNode;
	
	/*rightNode로 복사된 원소들을 leftNode에서 제거함*/
	for (int i = splitFactor + 1; i < ENTRYSIZE; i++) {
		memset(&leftNode->nodeEntry[i], 0, sizeof(struct entry));
	}

	leftNode->entryCount = splitFactor + 1;
	rightNode->entryCount = ENTRYSIZE - (splitFactor + 1);
	
	/*ParentNode를 지정하는 과정*/
	if (parentSet[parentIndex] == NULL) {   //분할되는 노드가 기존의 루트노드였을 경우
		parentNode = createNode(); //ParentNode 생성
		root = parentNode;
		depth++;
	}
	
	else { //ParentNode는 기존 노드의 상위 노드
		parentNode = parentSet[parentIndex];
		
		/*parentNode가 꽉 찼을 경우 스플릿*/
		if (parentNode->entryCount == ENTRYSIZE) {
			parentNode = split(parentNode, splitKey, parentSet, parentIndex - 1, false);
		}
	}
	
	/*---------여기에 parent노드에 삽입되는 부분 작성--------*/
	internalNodeInsert(parentNode, splitKey, leftNode, rightNode);

	if (key < splitKey) {
		return leftNode;
	}
	else {
		return rightNode;
	}
}

void internalNodeInsert(node *internalNode, float key, node *leftNode, node *rightNode) {
	entry newEntry;
	blockPointer ptr;
	
	/*삽입될 엔트리가 가리키는 포인터는 왼쪽 child 노드*/
	ptr.childNode = leftNode;
	/*엔트리 생성*/
	//createNewEntry(&newEntry, internalNode->isLeaf, key, ptr);
	createNewEntry(&newEntry, false, key, ptr);
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
	int currentDepth = 0;
	while (1) {
		//if (currentNode->isLeaf) break;
		if (currentDepth == depth) break;
		if (currentNode->entryCount > 0) {
			for (int i = 0; i < currentNode->entryCount; i++) {
				count++;
				if (key <= currentNode->nodeEntry[i].score) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNode = currentNode->nodeEntry[i].a.childNode;
					currentDepth++;
					break;
				}
				if (count == currentNode->entryCount) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNode = currentNode->elseNode;
					currentDepth++;
					break;
				}
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
	int count = 0;
	int index;
	vector<node*> parentSet;
	node *parentNode = NULL;
	int currentDepth = 0;

	/*key값이 이미 트리에 존재하는지 체크*/
	if (search(key) >= 0) {
		return false;
	}

	/*엔트리를 삽입할 리프노드를 찾는 단계*/
	while (1) {
		parentSet.push_back(parentNode);
		//if (tmpNode->isLeaf) break;
		if (currentDepth == depth) break;
		if (tmpNode->entryCount > 0) {
			for (int i = 0; i < tmpNode->entryCount; i++) {
				count++;
				if (key <= tmpNode->nodeEntry[i].score) {
					parentNode = tmpNode;
					tmpNode = tmpNode->nodeEntry[i].a.childNode;
					currentDepth++;
					break;
				}
				if (count == tmpNode->entryCount) {
					//if (tmpNode->isLeaf) break;
					if (currentDepth == depth) break;
					parentNode = tmpNode;
					tmpNode = tmpNode->elseNode;
					currentDepth++;
					break;
				}
			}
			count = 0;
		}
	}

	/*해당 리프가 꽉 찼을 경우 노드 분할*/
	if (tmpNode->entryCount == ENTRYSIZE) {
		tmpNode = split(tmpNode, key, parentSet, parentSet.size() - 1, true);
	}

	/*새로운 엔트리 생성*/
	createNewEntry(&newEntry, true, key, ptr);
	
	/*노드에 삽입할 위치 지정*/
	index = tmpNode->entryCount;
	for (int i = 0; i < tmpNode->entryCount; i++) {
		if (key <= tmpNode->nodeEntry[i].score) {
			index = i;
			break;
		}
	}
	/*노드에 엔트리를 삽입*/
	//if (tmpNode->isLeaf) { 
		/*노드가 리프일 경우(무조건 리프일듯)*/		
	for (int i = tmpNode->entryCount; i > index; i--) {
		memcpy(&tmpNode->nodeEntry[i], &tmpNode->nodeEntry[i - 1], sizeof(struct entry)); //엔트리를 하나씩 right shift
	}
	memset(&tmpNode->nodeEntry[index], 0, sizeof(struct entry)); //삽입할 엔트리 번지 초기화
	memcpy(&tmpNode->nodeEntry[index], &newEntry, sizeof(struct entry)); //엔트리 삽입
	tmpNode->entryCount++;
	//}

	return true;
}

node* sequencialSearch(int k) {
	node *tmp;
	tmp = root;
	int currentDepth = 0;
	tmp = seqSearchDetail(root, k, currentDepth);
	for (int i = 1; i < k; i++) {
		if (tmp->elseNode != NULL) {
			tmp = tmp->elseNode;
		}
		else
			return NULL;
	}
	return tmp;
}
node* seqSearchDetail(node *currentNode, int k, int currentDepth) {
	if (currentDepth != depth) {
		if (currentNode->entryCount != 0) {
			return seqSearchDetail(currentNode->nodeEntry[0].a.childNode, k, currentDepth + 1);
		}
		else
			return NULL;
	}
	else
		return currentNode;
}



