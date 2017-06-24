#include "BPTree.h"
#include "hash.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

node *root = createNode();
hashNode *hashRoot = createHashNode();
FILE* pFile = NULL;
const bool studentTable = false;
const bool profTable = true;
bool tableType = studentTable;
FILE* studentHashTable = NULL;
FILE* profHashTable = NULL;
FILE* stDB = NULL;
FILE* proDB = NULL;
hashTable stTable;
hashTable prTable;

void makeStudentDB() {
	makeFile();
	fopen_s(&pFile, "Students_score.idx", "r+b");
	ifstream DB("student_data.csv");
	initNode(root);
	initNode(hashRoot);
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
		if (i % 128 == 0) {
			printf("");
		}
		insert(st, hashRoot);
		a.blockNum = st.studentID;
		insert(st.score, a);
	}
	fclose(pFile);
	//createDB(hashRoot);
	makeHashTable(&stTable, hashRoot);
	createHashTableFile(&stTable);
	
}

void makeProfessorDB() {
	tableType = profTable;
	makeFile();
	fopen_s(&pFile, "Professor_Salary.idx", "r+b");
	root = createNode();
	ifstream DB("prof_data.csv");
	initNode(root);
	initNode(hashRoot);
	int b = 0;
	int count;
	int debugcount = 0;
	char input[50];
	char* token;
	char* context;
	professor prof;
	blockPointer a;
	DB.getline(input, sizeof(char) * 13);
	count = atoi(input);
	for (int i = 0; i < count && !DB.eof(); i++) {

		DB.getline(input, sizeof(char) * 50);
		token = strtok_s(input, ",", &context);
		for (int i = 0; i < 19; i++) {
			prof.name[i] = token[i];
		}
		prof.name[19] = 0;
		token = strtok_s(NULL, ",", &context);
		prof.professorID = atoi(token);
		token = strtok_s(NULL, ",", &context);
		prof.salary = atoi(token);
		if (i == 127) {
			printf("");
		}
		//insert(prof, hashRoot);
		a.blockNum = prof.professorID;
		insert(prof.salary, a);
	}
	fclose(pFile);
	//createDB(hashRoot);
	//createHashTableFile(hashRoot);

}


void scoreRangeSearch(float min, float max) {
	fopen_s(&pFile, "Students_score.idx", "rb");
	loadFile();
	rangeSearch(min, max);
	fclose(pFile);
}

void salaryRangeSearch(int min, int max) {
	float fmin = min;
	float fmax = max;
	fopen_s(&pFile, "Professor_Salary.idx", "rb");
	loadFile();
	rangeSearch(fmin, fmax);
	fclose(pFile);
}

void studentExactSearch(int key) {
	
	char *binaryKey;
	int blockNum;
	binaryKey = uintToBinary(key);
	int count = 0;
	hashNode *tmp;
	tmp = hashRoot;
	while(1){
		if (hashCompare(tmp->key, binaryKey, tmp->prefix)) {
					
		}
	}
}

void profExactSearch(int key) {

}

void join() {

}

void query() {
	fopen_s(&stDB, "stdents.DB", "rb");
	ifstream DB("query.csv");
	int count;
	char input[50];
	char* token;
	char* context;
	char queryState[20] = { 0, };
	char tableName[15] = { 0, };
	char attributeName[15] = { 0, };
	int min, max, id;
	float fmin, fmax;

	DB.getline(input, sizeof(char) * 13);
	count = atoi(input);
	for (int i = 0; i < count && !DB.eof(); i++) {

		DB.getline(input, sizeof(char) * 50);
		token = strtok_s(input, ",", &context);
		for (int i = 0; i < 19; i++) {
			queryState[i] = token[i];
		}
		queryState[19] = 0;

		token = strtok_s(NULL, ",", &context);
		for (int i = 0; i < 14; i++) {
			tableName[i] = token[i];
		}
		tableName[15] = 0;

		token = strtok_s(NULL, ",", &context);
		for (int i = 0; i < 14; i++) {
			attributeName[i] = token[i];
		}
		attributeName[14] = 0;

		if (strcmp("Search-Exact", queryState) == 0) {
			if (strcmp("Professors", tableName) == 0) {
				token = strtok_s(NULL, ",", &context);
				id = atoi(token);
				profExactSearch(id);
			}
			else if(strcmp("Students", tableName) == 0) {
				token = strtok_s(NULL, ",", &context);
				id = atoi(token);
				studentExactSearch(id);
			}
		}
		else if(strcmp("Search-Range", queryState) == 0) {
			if (strcmp("Professors", tableName) == 0) {
				token = strtok_s(NULL, ",", &context);
				min = atoi(token);
				token = strtok_s(NULL, ",", &context);
				max = atoi(token);
				salaryRangeSearch(min, max);
			}
			else if (strcmp("Students", tableName) == 0) {
				token = strtok_s(NULL, ",", &context);
				fmin = atof(token);
				token = strtok_s(NULL, ",", &context);
				fmax = atof(token);
				scoreRangeSearch(fmin, fmax);
			}
		}
		else if(strcmp("Join", queryState) == 0) {
			join();
		}
	}
}


int main() {
	makeStudentDB();
	//makeProfessorDB();
	//salaryRangeSearch(100003, 100530);

	int prefix;
	char key[33];
	int blockNum;

	return 0;
}

