#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include "BPTree.h"
using namespace std;
int depth = 0;
int btNodeCount = 0;
int btRootNodeNum = 1;

node* createNode() {
	node *newNode;
	newNode = (struct node *)malloc(sizeof(struct node));
	memset(newNode, 0, sizeof(struct node));
	return newNode;
}

void initNode(node *initialNode) {
	initialNode->elseNode = NULL;
	initialNode->entryCount = 0;
	depth = 0;
	btNodeCount = 1;
	btRootNodeNum = 1;
	writeCommonInfo();
	writeNodeInfo(initialNode, btRootNodeNum);
}

void copyNodeEntry(node *dst, node *src, int dstFirst, int srcFirst, int size) {
	size = size * sizeof(struct entry);
	memcpy(&dst->nodeEntry[dstFirst], &src->nodeEntry[srcFirst], size);
}

int split(node *nodeToSplit, float key, vector<unsigned int> &parentSet, int parentIndex, bool isLeaf, int leftNodeNum) {
	int splitFactor = ((ENTRYSIZE -1) >> 1);
	float splitKey = nodeToSplit->nodeEntry[splitFactor].score;
	int rightNodeNum;
	int parentNodeNum;

	node *parentNode = NULL;
	node *leftNode = NULL;
	node *rightNode = NULL;
	
	/*leftNode는 기존의 노드를 그대로 사용
	rightNode는 새로 생성하여 leftNode의
	split 지점 이후의 원소들을 복사함*/
	leftNode = nodeToSplit;	//
	//rightNode = createNode(nodeToSplit->isLeaf);	
	rightNode = createNode();
	btNodeCount++;
	rightNodeNum = btNodeCount;
	copyNodeEntry(rightNode, nodeToSplit, 0, splitFactor + 1, ENTRYSIZE - splitFactor + 1);
		
	if (isLeaf) {
		rightNode->elseNode = leftNode->elseNode;
		leftNode->elseNode = rightNodeNum;
	}
	else leftNode->elseNode = leftNode->nodeEntry[splitFactor + 1].a.childNode;
	
	/*rightNode로 복사된 원소들을 leftNode에서 제거함*/
	for (int i = splitFactor + 1; i < ENTRYSIZE; i++) {
		memset(&leftNode->nodeEntry[i], 0, sizeof(struct entry));
	}

	leftNode->entryCount = splitFactor + 1;
	rightNode->entryCount = ENTRYSIZE - (splitFactor + 1);
	
	writeNodeInfo(leftNode, leftNodeNum);
	writeNodeInfo(rightNode, rightNodeNum);
	writeCommonInfo();

	/*ParentNode를 지정하는 과정*/
	if (parentSet[parentIndex] == 0) {   //분할되는 노드가 기존의 루트노드였을 경우
		parentNode = createNode(); //ParentNode 생성
		btNodeCount++;
		parentNodeNum = btNodeCount;
		btRootNodeNum = parentNodeNum;
		depth++;
		writeCommonInfo();
	}	
	else { //ParentNode는 기존 노드의 상위 노드
		parentNodeNum = parentSet[parentIndex];
		parentNode = createNode();
		loadNodeInfo(parentNode, parentNodeNum);
		/*parentNode가 꽉 찼을 경우 스플릿*/
		if (parentNode->entryCount == ENTRYSIZE) {
			parentNodeNum = split(parentNode, splitKey, parentSet, parentIndex - 1, false, parentNodeNum);
			loadNodeInfo(parentNode, parentNodeNum);
		}
	}
	
	/*---------여기에 parent노드에 삽입되는 부분 작성--------*/
	internalNodeInsert(parentNode, splitKey, leftNodeNum, rightNodeNum);
	writeNodeInfo(parentNode, parentNodeNum);
	
	//free(parentNode);
	//free(rightNode);
	//free(leftNode);
	if (key <= splitKey) {
		return leftNodeNum;
	}
	else {
		return rightNodeNum;
	}
}

void internalNodeInsert(node *internalNode, float key, int leftNodeNum, int rightNodeNum) {
	entry newEntry;
	blockPointer ptr;
	
	/*삽입될 엔트리가 가리키는 포인터는 왼쪽 child 노드*/
	ptr.childNode = leftNodeNum;
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
		internalNode->elseNode = rightNodeNum;
	}
	/*엔트리가 노드의 중간 어딘가 삽입될 경우*/
	else {
		internalNode->nodeEntry[index + 1].a.childNode = rightNodeNum;
	}
	internalNode->entryCount++;
}

int search(float key) {
	node root;
	loadNodeInfo(&root, btRootNodeNum);
	return searchDetail(&root, key);
}

int searchDetail(node *currentNode, float key)
{
	int count = 0;
	int currentDepth = 0;
	int currentNodeNum = 0;
	while (1) {
		//if (currentNode->isLeaf) break;
		if (currentDepth == depth) break;
		if (currentNode->entryCount > 0) {
			for (int i = 0; i < currentNode->entryCount; i++) {
				count++;
				if (key <= currentNode->nodeEntry[i].score) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNodeNum = currentNode->nodeEntry[i].a.childNode;
					loadNodeInfo(currentNode, currentNodeNum);
					currentDepth++;
					break;
				}
				if (count == currentNode->entryCount) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNodeNum = currentNode->elseNode;
					loadNodeInfo(currentNode, currentNodeNum);
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

int rangeSearch(float min, float max) {
	loadFile();
	node *currentNode = createNode();
	loadNodeInfo(currentNode, btRootNodeNum);

	int count = 0;
	int currentDepth = 0;
	int currentNodeNum = 0;
	while (1) {
		//if (currentNode->isLeaf) break;
		if (currentDepth == depth) break;
		if (currentNode->entryCount > 0) {
			for (int i = 0; i < currentNode->entryCount; i++) {
				count++;
				if (min <= currentNode->nodeEntry[i].score) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNodeNum = currentNode->nodeEntry[i].a.childNode;
					loadNodeInfo(currentNode, currentNodeNum);
					currentDepth++;
					break;
				}
				if (count == currentNode->entryCount) {
					//if (currentNode->isLeaf) break;
					if (currentDepth == depth) break;
					currentNodeNum = currentNode->elseNode;
					loadNodeInfo(currentNode, currentNodeNum);
					currentDepth++;
					break;
				}
			}
			count = 0;
		}
	}
	bool more = true;
	int blockNum;
	int resultCount = 0;
	while (more) {
		for (int i = 0; i < currentNode->entryCount; i++) {
			if (min <= currentNode->nodeEntry[i].score)
			{
				if (max >= currentNode->nodeEntry[i].score) {
					blockNum = currentNode->nodeEntry[i].a.blockNum;
					resultCount++;
					if (tableType == studentTable)
						studentExactSearch(blockNum);
					else
						profExactSearch(blockNum);
				}
				else {
					more = false;
					break;
				}
			}			
		}		
		if (!more) break;

		if(currentNode->elseNode != NULL && currentNode->elseNode != 0)
			loadNodeInfo(currentNode, currentNode->elseNode);
		else 
			break;
	}

	free(currentNode);
	return resultCount;
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
	node *tmpNode = createNode();
	int count = 0;
	int index;
	vector<unsigned int> parentSet;
	int parentNode = 0;
	int currentDepth = 0;
	int tmpNodeNum = btRootNodeNum;

	loadNodeInfo(tmpNode, btRootNodeNum);

	/*key값이 이미 트리에 존재하는지 체크*/
	//if (search(key) >= 0) {
		//return false;
	//}

	/*엔트리를 삽입할 리프노드를 찾는 단계*/
	while (1) {
		parentSet.push_back(parentNode);
		if (currentDepth == depth) break;
		if (tmpNode->entryCount > 0) {
			for (int i = 0; i < tmpNode->entryCount; i++) {
				count++;
				if (key <= tmpNode->nodeEntry[i].score) {
					parentNode = tmpNodeNum;
					tmpNodeNum = tmpNode->nodeEntry[i].a.childNode;
					loadNodeInfo(tmpNode, tmpNodeNum);
					currentDepth++;
					break;
				}
				if (count == tmpNode->entryCount) {
					if (currentDepth == depth) break;
					parentNode = tmpNodeNum;
					tmpNodeNum = tmpNode->elseNode;
					loadNodeInfo(tmpNode, tmpNodeNum);
					currentDepth++;
					break;
				}
			}
			count = 0;
		}
	}

	/*해당 리프가 꽉 찼을 경우 노드 분할*/
	if (tmpNode->entryCount == ENTRYSIZE) {
		tmpNodeNum = split(tmpNode, key, parentSet, parentSet.size() - 1, true, tmpNodeNum);
		//tmpNode = createNode();
		loadNodeInfo(tmpNode, tmpNodeNum);
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
	/*이 부분에 갱신된 노드를 파일에 쓰도록 함. 그다음 메모리 해제.*/
	writeNodeInfo(tmpNode, tmpNodeNum);
	free(tmpNode);
	return true;
}




