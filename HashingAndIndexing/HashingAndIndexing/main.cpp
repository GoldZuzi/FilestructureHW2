#include "BPTree.h"

node *root = createNode();

int main() {
	initNode(root);
	int b = 0;
	blockPointer a;
	a.blockNum = 4;
	insert(3.3, a);
	a.blockNum = 5;
	insert(3.4, a);
	a.blockNum = 6;
	insert(3.5, a);

	a.blockNum = 7;
	insert(3.6, a);
	a.blockNum = 8;
	insert(3.7, a);
	a.blockNum = 9;
	insert(3.8, a);

	a.blockNum = 10;
	insert(3.9, a);
	a.blockNum = 11;
	insert(4.0, a);
	a.blockNum = 12;
	insert(3.55, a);

	a.blockNum = 17;
	insert(3.42, a);
	a.blockNum = 20;
	insert(3.58, a);

	b = search(3.3);
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
	printf("%d ", b);
	return 0;
}
