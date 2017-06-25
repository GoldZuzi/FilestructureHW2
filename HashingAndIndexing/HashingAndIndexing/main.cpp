#include "BPTree.h"
#include "hash.h"
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

node *root = createNode();
hashNode *hashRoot = createHashNode();
hashNode *prHashRoot = createHashNode();
FILE* pFile = NULL;
const bool studentTable = false;
const bool profTable = true;
bool tableType = studentTable;
FILE* studentHashTable = NULL;
FILE* profHashTable = NULL;
FILE* stDB = NULL;
FILE* proDB = NULL;
FILE* resultFile = NULL;
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
	createDB(hashRoot);
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
	initNode(prHashRoot);
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
		insert(prof, prHashRoot);
		a.blockNum = prof.professorID;
		insert(prof.salary, a);
	}
	fclose(pFile);
	createDB(prHashRoot);
	makeHashTable(&prTable, prHashRoot);
	createHashTableFile(&prTable);

}


void scoreRangeSearch(float min, float max) {
	tableType = studentTable;
	fopen_s(&pFile, "Students_score.idx", "rb");
	loadFile();
	rangeSearch(min, max);
	fclose(pFile);
}

void salaryRangeSearch(int min, int max) {
	tableType = profTable;
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
	int index;
	binaryKey = uintToBinary(key);
	int prefix = stTable.prefix;
	memset(binaryKey, '0', 32 - prefix);
	index = binaryToUint(binaryKey);
	blockNum = stTable.blockNum[index];
	student st[MAXNUM];
	int pos = 4096 * blockNum;
	fseek(stDB, pos, SEEK_SET);
	fread(st, sizeof(struct student), MAXNUM, stDB);
	rewind(stDB);
	for (int i = 0; i < MAXNUM; i++) {
		if (key == st[i].studentID) {
			fprintf(resultFile, "%s, %d, %f, %d\n", st[i].name, st[i].studentID, st[i].score, st[i].advisorID);
			//cout << st[i].name << "  " << st[i].studentID << "  " << st[i].score << "  " << st[i].advisorID << endl; //여기에 파일 추가하는 부분 추가
			break;
		}
	}
}

void profExactSearch(int key) {
	char *binaryKey;
	int blockNum;
	int index;
	binaryKey = uintToBinary(key);
	int prefix = prTable.prefix;
	memset(binaryKey, '0', 32 - prefix);
	index = binaryToUint(binaryKey);
	blockNum = prTable.blockNum[index];
	professor pf[146];
	int pos = 4096 * blockNum;
	fseek(proDB, pos, SEEK_SET);
	fread(pf, sizeof(struct professor), 146, proDB);
	rewind(proDB);
	for (int i = 0; i < 146; i++) {
		if (key == pf[i].professorID) {
			fprintf(resultFile, "%s, %d, %d\n", pf[i].name, pf[i].professorID, pf[i].salary);
			//cout << pf[i].name << "  " << pf[i].professorID << "  " << pf[i].salary << endl; //여기에 파일 추가하는 부분 추가
			break;
		}
	}
}

void join() {	
	student st[MAXNUM];
	professor pf[146];
	int stBlockNum = 0;
	int pfBlockNum = 0;
	char a[9];
	while(1) {
		fread(pf, sizeof(struct professor), 146, proDB);
		fread(a, 8, 1, proDB);
		if (feof(proDB) != 0)	break;
		while(1) {
			fread(st, sizeof(struct student), MAXNUM, stDB);
			if (feof(stDB) != 0)	break;
			for (int k = 0; k < 146 && pf[k].professorID != 0; k++) {
				for (int l = 0; l < MAXNUM && st[l].studentID != 0; l++) {
					if (pf[k].professorID == st[l].studentID) {
						fprintf(resultFile, "%s, %d, %d, %s, %d, %f\n", pf[k].name, pf[k].professorID, pf[k].salary, st[l].name, st[l].studentID, st[l].score);
					}
				}
			}
		}
		rewind(stDB);
	}
	rewind(proDB);	
}

void query() {
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
			fprintf(resultFile, "---------------------------------- Professor exact-search result ----------------------------------\n");
			if (strcmp("Professors", tableName) == 0) {
				token = strtok_s(NULL, ",", &context);
				id = atoi(token);
				profExactSearch(id);
			}
			else if(strcmp("Students", tableName) == 0) {
				fprintf(resultFile, "---------------------------------- Student exact-search result ----------------------------------\n");
				token = strtok_s(NULL, ",", &context);
				id = atoi(token);
				studentExactSearch(id);
			}
		}
		else if(strcmp("Search-Range", queryState) == 0) {
			if (strcmp("Professors", tableName) == 0) {
				fprintf(resultFile, "---------------------------------- Professor range-search result ----------------------------------\n");
				token = strtok_s(NULL, ",", &context);
				min = atoi(token);
				token = strtok_s(NULL, ",", &context);
				max = atoi(token);
				salaryRangeSearch(min, max);
			}
			else if (strcmp("Students", tableName) == 0) {
				fprintf(resultFile, "---------------------------------- Student range-search result ----------------------------------\n");
				token = strtok_s(NULL, ",", &context);
				fmin = atof(token);
				token = strtok_s(NULL, ",", &context);
				fmax = atof(token);
				scoreRangeSearch(fmin, fmax);
			}
		}
		else if(strcmp("Join", queryState) == 0) {
			fprintf(resultFile, "----------------------------------------- Join result -----------------------------------------\n");
			join();
		}
	}
}


int main() {
	//makeStudentDB();
	//makeProfessorDB();
	fopen_s(&stDB, "students.DB", "rb");
	fopen_s(&proDB, "professors.DB", "rb");
	fopen_s(&resultFile, "query.result", "wt");
	loadHashTable(&stTable, studentTable);
	loadHashTable(&prTable, profTable);		
	//studentExactSearch(30585);
	scoreRangeSearch(6.19001, 6.3);
	salaryRangeSearch(100003, 100530);
	join();
	fclose(proDB);
	fclose(stDB);
	return 0;
}

