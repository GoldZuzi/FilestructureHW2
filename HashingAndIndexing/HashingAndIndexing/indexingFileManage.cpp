#include "BPTree.h"

void makeFile() {
	if (tableType == studentTable)
		fopen_s(&pFile, "Students_score.idx", "wb");
	else
		fopen_s(&pFile, "Professor_Salary.idx", "wb");
	fwrite(&depth, 4, 1, pFile);
	fwrite(&btNodeCount, 4, 1, pFile);
	fwrite(&btRootNodeNum, 4088, 1, pFile);
	fclose(pFile);
}

void writeCommonInfo() {
	/*if (tableType == studentTable)
		fopen_s(&pFile, "Students_score.idx", "r+b");
	else
		fopen_s(&pFile, "Professor_Salary.idx", "r+b");*/
	fwrite(&depth, 4, 1, pFile);
	fwrite(&btNodeCount, 4, 1, pFile);
	fwrite(&btRootNodeNum, 4088, 1, pFile);
	rewind(pFile);
	//fclose(pFile);
}

void writeNodeInfo(node *currentNode, int nodeBlockNum) {
	int pos = 4096 * nodeBlockNum;
	/*if(tableType == studentTable)
		fopen_s(&pFile, "Students_score.idx", "r+b");
	else
		fopen_s(&pFile, "Professor_Salary.idx", "r+b");*/
	fseek(pFile, pos, SEEK_SET);
	fwrite(currentNode, sizeof(node), 1, pFile);
	rewind(pFile);
	//fclose(pFile);
}

void loadFile() {
	void *skip = new int[1023];
	/*if (tableType == studentTable)
		fopen_s(&pFile, "Students_score.idx", "rb");
	else
		fopen_s(&pFile, "Professor_Salary.idx", "rb");*/
	fread(&depth, sizeof(int), 1, pFile);
	fread(&btNodeCount, sizeof(int), 1, pFile);
	fread(&btRootNodeNum, sizeof(int), 1, pFile);
	fread(skip, sizeof(int), 1021, pFile);
	rewind(pFile);
	//fclose(pFile);
}

void loadNodeInfo(node *currentNode, int nodeBlockNum) {
	int pos = 4096 * nodeBlockNum;
	//if (tableType == studentTable)
	//	fopen_s(&pFile, "Students_score.idx", "rb");
	//else
	//	fopen_s(&pFile, "Professor_Salary.idx", "rb");
	fseek(pFile, pos, SEEK_SET);
	fread(&currentNode->entryCount, sizeof(int), 1, pFile);
	fread(currentNode->nodeEntry, sizeof(entry), ENTRYSIZE, pFile);
	fread(&currentNode->elseNode, sizeof(int), 1, pFile);
	rewind(pFile);
	//fclose(pFile);
}
