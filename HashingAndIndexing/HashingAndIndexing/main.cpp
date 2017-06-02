#include "BPTree.h"
#include "hash.h"

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
node *root = createNode();
hashNode *hashRoot = createHashNode();
FILE* pFile = NULL;

int main() {
	
	ifstream DB("sampleData.csv");
	initNode(root);
	initNode(hashRoot);
	//loadFile();
	int b = 0;
	int count;
	int debugcount = 0;
	char input[50];
	char* token;
	char* context;
	student st;
	blockPointer a;
	DB.getline(input, sizeof(char) * 13);
	count = atoi(input);
	for (int i = 0; i < count && !DB.eof(); i++) {
	
		DB.getline(input, sizeof(char) * 50);	
		token = strtok_s(input, ",", &context);
		for (int i = 0; i < 19; i++) {
			st.name[i] = token[i];
		}
		st.name[19] = 0;
		token = strtok_s(NULL, ",", &context);
		st.studentID = atoi(token);
		token = strtok_s(NULL, ",", &context);
		st.score = atof(token);
		token = strtok_s(NULL, ",", &context);
		st.advisorID = atoi(token);
		insert(st, hashRoot);
		a.blockNum = st.studentID;
		insert(st.score, a);
	
	}
	makeFile();
	createDB(hashRoot);
	createHashTableFile(hashRoot);
	printf("%d", sizeof(struct node));

	int kthPos = 0;
	node* kthNode;
	printf("원하는 리프노드 번호 입력: ");
	scanf_s("%d", &kthPos);
	fflush(stdin);
	kthNode = sequencialSearch(kthPos);
	if (kthNode != NULL) {
		for (int i = 0; i < kthNode->entryCount; i++) {
			printf("%f  ", kthNode->nodeEntry[i].score);
		}
	}
	else
		printf("there is no %dth node\n", kthPos);
	

	return 0;
}

