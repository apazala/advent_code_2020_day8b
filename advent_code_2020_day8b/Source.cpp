#include <stdio.h>
#include <stdlib.h>
#define K 26
#define NO_WORD -1

typedef struct trie_node {
	struct trie_node* children[K];
	int word_index;
}trie_node;

trie_node *new_trie_node() {
	trie_node *node = (trie_node *)calloc(1, sizeof(trie_node));
	node->word_index = NO_WORD;
	return node;
}

void add_word(trie_node *root, char *word, int val) {
	trie_node *curr = root;
	for (; *word; word++) {
		int ind = *word - 'a';
		if (!curr->children[ind]) {
			curr->children[ind] = new_trie_node();
		}
		curr = curr->children[ind];
	}
	curr->word_index = val;
}

#define NINS 3
char instructions[NINS][4] = { "nop", "jmp", "acc" };
typedef enum { NOP, JMP, ACC } ins_code;

void init(trie_node *trie) {
	for (int i = 0; i < NINS; i++)
		add_word(trie, instructions[i], i);
}

int search_trie(trie_node *trie, char *word) {
	for (; *word; word++) {
		int ind = *word - 'a';
		trie = trie->children[ind];
		if (!trie)
			return NO_WORD;
	}

	return trie->word_index;
}

#define MAX_LINES 700

ins_code ins_codes[MAX_LINES];
int ins_values[MAX_LINES];
int visited[MAX_LINES];

int clines = 0;

int back_track(int i, int *pacc, int canswitch) {
	if (i == clines) return 1;
	if (i > clines || i < 0) return -1;
	if (visited[i]) return -1;

	visited[i] = 1;
	if (ins_codes[i] == ACC) {
		*pacc += ins_values[i];
		if (back_track(i + 1, pacc, canswitch) == 1) return 1;

		//backtrack
		*pacc -= ins_values[i];
	}
	else if (ins_codes[i] == NOP) {
		if (back_track(i + 1, pacc, canswitch) == 1) return 1;
		if (canswitch) //NOP -> JMP
			if (back_track(i + ins_values[i], pacc, 0) == 1) return 1;
	}
	else {
		if (back_track(i + ins_values[i], pacc, canswitch) == 1) return 1;
		if (canswitch) //JMP -> NOP
			if (back_track(i + 1, pacc, 0) == 1) return 1;
	}

	//backtrack
	visited[i] = 0;
	return -1;
}

int main() {
	char instruction[5];
	int value;
	trie_node *trie = new_trie_node();

	init(trie);

	FILE *fin = fopen("input.txt", "r");
	if (!fin) {
		fprintf(stderr, "Can't open input file\n");
		return 1;
	}

	while (fscanf(fin, "%s %d", instruction, &value) == 2) {
		ins_codes[clines] = (ins_code)search_trie(trie, instruction);
		ins_values[clines] = value;
		clines++;
	}

	fclose(fin);

	int acc = 0;

	back_track(0, &acc, 1);


	printf("%d\n", acc);

	return 0;
}