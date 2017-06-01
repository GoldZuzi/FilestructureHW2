#include "BPTree.h"

void makeFile() {
	fopen_s(&pFile, "Students_score.idx", "wb");
	fwrite(&depth, 4096, 1, pFile);
	writeNodeInfo(root, 0);
	fclose(pFile);
}

void writeNodeInfo(node *currentNode, int currentDepth) {
	fwrite(currentNode, sizeof(node), 1, pFile);
	if (currentDepth != depth) {
		for (int i = 0; i < currentNode->entryCount; i++) {
			writeNodeInfo(currentNode->nodeEntry[i].a.childNode, currentDepth + 1);
		}
		writeNodeInfo(currentNode->elseNode, currentDepth + 1);
	}
}

void loadFile() {
	void *skip = new int[1023];
	fopen_s(&pFile, "Students_score.idx", "rb");
	fread(&depth, sizeof(int), 1, pFile);
	fread(skip, sizeof(int), 1023, pFile);
	loadNodeInfo(root, 0);
	fclose(pFile);
}

void loadNodeInfo(node *currentNode, int currentDepth) {
	//node *tmp;
	fread(&currentNode->entryCount, sizeof(int), 1, pFile);
	fread(currentNode->nodeEntry, sizeof(entry), ENTRYSIZE, pFile);
	fread(&currentNode->elseNode, sizeof(node*), 1, pFile);
	if (!feof(pFile)) {
		if (currentDepth != depth) {
			for (int i = 0; i < currentNode->entryCount; i++) {
				currentNode->nodeEntry[i].a.childNode = createNode();
				loadNodeInfo(currentNode->nodeEntry[i].a.childNode, currentDepth + 1);
			}
			currentNode->elseNode = createNode();
			loadNodeInfo(currentNode->elseNode, currentDepth + 1);
		}
	}
}
