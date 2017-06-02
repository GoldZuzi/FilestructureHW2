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
	//a.blockNum = 4;
	//insert(3.3, a);
	//a.blockNum = 5;
	//insert(3.4, a);
	//a.blockNum = 6;
	//insert(3.5, a);

	//a.blockNum = 7;
	//insert(3.6, a);
	//a.blockNum = 8;
	//insert(3.7, a);
	//a.blockNum = 9;
	//insert(3.8, a);

	//a.blockNum = 10;
	//insert(3.9, a);
	//a.blockNum = 11;
	//insert(4.0, a);
	//a.blockNum = 12;
	//insert(3.55, a);

	//a.blockNum = 17;
	//insert(3.42, a);
	//a.blockNum = 20;
	//insert(3.58, a);

	//makeFile();

	node* kthNode;
	kthNode = sequencialSearch(1);
	if (kthNode != NULL) {
		for (int i = 0; i < kthNode->entryCount; i++) {
			printf("%f  ", kthNode->nodeEntry[i].score);
		}
	}
	else
		printf("there is no %dth node\n", 6);

	/*b = search(3.3);
	printf("%d ", b);
	b = search(3.4);
	printf("%d ", b);
	b = search(3.42);
	printf("%d ", b);
	b = search(3.5);
	printf("%d ", b);
	b = search(3.55);
	printf("%d ", b);
	b = search(3.58);
	printf("%d ", b);
	b = search(3.6);
	printf("%d ", b);
	b = search(3.7);
	printf("%d ", b);
	b = search(3.8);
	printf("%d ", b);
	b = search(3.9);
	printf("%d ", b);
	b = search(4.0);
	printf("%d ", b);*/
	//b = sizeof(struct node);
	//printf("%d ", b);
	

	return 0;
}

