#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MEMORYCOLUMN 16
#define MEMORYROW 65536
#define MEMORYSIZE 1048576

// ------------- global variable and structure -------------------
typedef struct _Node *NodePointer;
typedef struct _Node {
	char inst[100];
	int num;
	NodePointer next;
} Node;

typedef struct _linkedList {
	NodePointer head, tail;
} linkedList;

typedef struct _opCode *opPointer;
typedef struct _opCode{
	char mnemonic[15];
	int code;
	int type;
	opPointer next;
}opCode;

typedef struct _Hashtable {
	opPointer head;
	int count;
} Hashtable;

unsigned char Memory[MEMORYROW][MEMORYCOLUMN];
linkedList *list_history;
Hashtable table[20];

// --------------- function declaration ----------------

void dataInit();
void shellInit();
void dataFree();

void inst_help();
void inst_dir();
void inst_history();
int inst_dump(char *inst, int *dumpCount, int CommaCount);
void inst_dump_print(int start, int end);
void inst_refine(char *token, char *inst_refined);
int inst_edit(char *inst_refined, int CommaCount);
int inst_fill(char *inst_refined, int CommaCount);
void inst_reset();
int inst_opcode(char* mnemonic, int index);
void inst_opcodelist();


void linkedList_append(char *inst);
void opCodeList_append(int code, char* mnemonic, int type, int index);
void opcodeFileOpen();
int HashFunc(char *mnemonic);

int CommaCheck(char* inst);
int Power(int base, int exp);
int DecStringToDec(char *token);
int HexStringToDec(char *token);

// ------------- function implementaion ------------------

void dataInit(){
	int i, j; // iteration variable

	// list init
	list_history = (linkedList*)malloc(sizeof(linkedList));
	list_history->head = NULL;
	list_history->tail = NULL;

	// memory init
	for (i=0 ; i<MEMORYROW ; i++){
		for (j=0 ; j<MEMORYCOLUMN ; j++)
			Memory[i][j] = 0;
	}

	// hastable init
	for (i=0 ; i<20 ; i++){
		table[i].head = NULL;
		table[i].count = 0;
	}
	return;
}

void dataFree(){
	int i;
	NodePointer iter_node, del_node;
	opPointer iter_op, del_op;

	// free instructions in history
	iter_node = list_history->head;
	while (iter_node != NULL){
		del_node = iter_node;
		iter_node = iter_node->next;
		free(del_node);
	}

	// free history
	free(list_history);

	// free hashtable
	for (i=0 ; i<20 ; i++){
		iter_op = table[i].head;
		while (iter_op != NULL){
			del_op = iter_op;
			iter_op = iter_op->next;
			free(del_op);
		}
	}
}


void shellInit(){
	char inst[100], inst_refined[30];
	char *token, *token_temp, *mnemonic;
	int dumpCount = 0, Stat, CommaCount;
	int i, j;

	opcodeFileOpen();

	while(1){
		printf("sicsim> ");

		// get instruction from user
		fgets(inst, sizeof(inst), stdin);
		inst[strlen(inst)-1] = '\0';

		// exception handling
		if (strcmp(inst, "\0") == 0)
			continue;
		CommaCount = CommaCheck(inst);
		

		// what instruction?
		token = strtok(inst, " \t");
		if (token == NULL)
			continue;

		// execute instruction
		if (strcmp(token, "help") == 0 || strcmp(token, "h") == 0){
			token_temp = token;
			token = strtok(NULL, " \t"); // exception handling
			if (token != NULL){
				puts(token);
				printf("Wrong instruction\n");
				continue;
			}
			linkedList_append(token_temp);
			inst_help();
		}
		else if (strcmp(token, "dir") == 0 || strcmp(token, "d") == 0) {
			token_temp = token;
			token = strtok(NULL, " \t");
			if (token != NULL){
				printf("Wrong instruction\n");
				continue;
			}
			linkedList_append(token_temp);
			inst_dir();
		}
		else if (strcmp(token, "quit") == 0 || strcmp(token, "q") == 0) {
			return;
		}
		else if (strcmp(token, "history") == 0 || strcmp(token, "hi") == 0) {
			linkedList_append(token);
			inst_history();
		}
		else if (strcmp(token, "dump") == 0 || strcmp(token, "du") == 0) {		
			inst_refine(token, inst_refined);
			Stat = inst_dump(inst_refined, &dumpCount, CommaCount);
			if (Stat)
				linkedList_append(inst_refined);
			for (i=0 ; i<30 ; i++)
				inst_refined[i] = '\0';
		}

		else if (strcmp(token, "edit") == 0 || strcmp(token, "e") == 0){
			inst_refine(token, inst_refined);
			Stat = inst_edit(inst_refined, CommaCount);
			if (Stat)
				linkedList_append(inst_refined);
		}
		else if (strcmp(token, "fill") == 0 || strcmp(token, "f") == 0){
			inst_refine(token, inst_refined);
			Stat = inst_fill(inst_refined, CommaCount);
			if (Stat)
				linkedList_append(inst_refined);
		}
		else if (strcmp(token, "reset") == 0){
			token_temp = token;
			token = strtok(NULL, " \t");
			if (token != NULL){
				printf("Wrong instruction\n");
				continue;
			}
			inst_reset();
			linkedList_append(token_temp);
		}
		else if (strcmp(token, "opcode") == 0){
			mnemonic = strtok(NULL, " \t");
			if (mnemonic == NULL){
				printf("Mnemonic missing\n");
				continue;
			}
			token_temp = strtok(NULL, " \t");
			if (token_temp != NULL){
				printf("Wrong instruction\n");
				continue;
			}
			Stat = inst_opcode(mnemonic, HashFunc(mnemonic));
			if (Stat){
				token[6] = ' '; 
				for (i=7, j=0 ; j<strlen(mnemonic) ; i++, j++)
					token[i] = mnemonic[j];
				token[i] = '\0';
				linkedList_append(token);
				
			}
			else
				printf("Not existing mnemonic\n");
		}
		else if (strcmp(token, "opcodelist") == 0){
			token_temp = token;
			token = strtok(NULL, " \t");
			if (token != NULL){
				printf("Wrong instruction\n");
				continue;
			}
			inst_opcodelist();
			linkedList_append(token_temp);
		}
		else {
			printf("Wrong instruction\n");
			continue;
		}
	}
}

void inst_help(){
	printf("h[elp]");
	printf("\nd[ir]");
	printf("\nq[uit]");
	printf("\nhi[story]");
	printf("\ndu[mp] [start, end]");
	printf("\ne[dit] address, value");
	printf("\nf[ill] start, end, value");
	printf("\nreset");
	printf("\nopcode mnemonic");
	printf("\nopcodelist\n");
}

void inst_dir(){
	DIR *dp = NULL;
    struct dirent *din = NULL; // file or directory structure
    struct stat buf; // stores file info
    dp = opendir(".");
	if (dp == NULL){
		printf("Cannot open directory\n");
		return;
	}
    while (1) {
        din = readdir(dp);
        if(din == NULL)
			break;
        lstat(din->d_name,&buf);
        printf("%s",din->d_name); // print file name
        if(S_ISDIR(buf.st_mode)) // if file is directory
			printf("/");
        else if(buf.st_mode & S_IXUSR) // if file is execution file
			printf("*");
        printf("     ");
    }
    printf("\n");
	return;
}

void inst_history(){
	Node *nodeptr;
	nodeptr = list_history->head;
	while (nodeptr != NULL){
		printf("       %-3d %s\n", nodeptr->num, nodeptr->inst);
		nodeptr = nodeptr->next;
	}
	return;
}

void linkedList_append(char *inst){
	// initialize newNode
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->next = NULL;
	strcpy(newNode->inst, inst);
	
	// append
	if (list_history->head == NULL){
		newNode->num = 1;
		list_history->head = newNode;
	}
	else {
		newNode->num = (list_history->tail->num) + 1;
		list_history->tail->next = newNode;
	}
	list_history->tail = newNode;
	
	return;
}

int inst_dump(char *inst, int *dumpCount, int CommaCount){
// Return : 1 if valid instruction
//          0 if failed
	int i, start=0, end=0, temp, mode=0;
	char inst_temp[30];
	char *token;
		
	strcpy(inst_temp, inst);
	token = strtok(inst_temp, " ");
	// 'start', 'end' separate 2 times
	for (i=0 ; i<2 ; i++){
		token = strtok(NULL, " ,");
		if (token == NULL) break;
		temp = HexStringToDec(token);
		if (temp == -1) // not hexadecimal format
			return 0;
		if (i==0){
			start = temp;
			mode = 1;
		}
		else if (i==1){
			end = temp;
			mode = 2;
		}
	}
	// find exception after separation
	token = strtok(NULL, " ");
	if (token != NULL){
		printf("Wrong instruction(wrong format)\n");
		return 0;
	}

	// 'dump'
	if (mode == 0){
		if (CommaCount != 0){
			printf("Wrong instruction(comma error)\n");
			return 0;
		}
		if (*dumpCount >= MEMORYSIZE){
			printf("Boundary error\n");
			return 0;
		}
		if (*dumpCount + 159 >= MEMORYSIZE)
			inst_dump_print(*dumpCount, MEMORYSIZE-1);
		else
			inst_dump_print(*dumpCount, *dumpCount+159);
		*dumpCount += 160;
	}
	// 'dump start'
	else if (mode == 1){
		if (CommaCount != 0){
			printf("Wrong instruction(comma error)\n");
			return 0;
		}
		if (start >= MEMORYSIZE){
			printf("Boundary error\n");
			return 0;
		}
		if (start + 159 >= MEMORYSIZE)
			inst_dump_print(start, MEMORYSIZE-1);
		else
			inst_dump_print(start, start+159);
	}
	// 'dump start, end'
	else if (mode == 2){
		if (CommaCount != 1){
			printf("Wrong instruction(comma error)\n");
			return 0;
		} 
		if (start > end){
			printf("'Start' bigger than 'End'\n");
			return 0;
		}
		if (end >= MEMORYSIZE){
			printf("Boundary error\n");
			return 0;
		}
		inst_dump_print(start, end);
	}
	else
		printf("Error in dump instruction\n");
	return 1;
}

void inst_dump_print(int start, int end){
	// *_index = i-th row in Memory
	int start_index = start / 16;
	int end_index = end / 16;
	int i;
	//printf("start = %d, end = %d\nstart_index = %d, end_index = %d\n",
	//		start, end, start_index, end_index);
	while (start_index <= end_index){
		// print memory address
		printf("%05X", start_index * 16);
		
		// print memory value
		for (i=0 ; i<16 ; i++){
			if (start_index == start / 16 && start > start_index * 16 + i)
				printf("   ");
			else if (start_index == end_index && end < end_index * 16 + i)
				printf("   ");
			else
				printf(" %02X", Memory[start_index][i]);
		}
		printf(" ; ");
		
		// print ascii value of memory
		for (i=0 ; i<16 ; i++){
			if (Memory[start_index][i] > 19 && Memory[start_index][i] < 0x7F)
				printf("%c", Memory[start_index][i]);
			else
				printf(".");
		}

		printf("\n");
		start_index++;
	}
	return;
}

int inst_edit(char *inst_refined, int CommaCount){
// Return : 1 if valid instruction
//          0 if failed
	
	int address, value;
	int row, col;
	char inst_temp[30];
	char* token;

	if (CommaCount != 1){
		printf("Wrong instruction(comma error)\n");
		return 0;
	} 
	strcpy(inst_temp, inst_refined);
	token = strtok(inst_temp, " ,");
	
	token = strtok(NULL, " ,");
	if (token == NULL){
		printf("Not enough values\n");
		return 0;
	}
	address = HexStringToDec(token);

	token = strtok(NULL, " ,");
	if (token == NULL){
		printf("Not enough values\n");
		return 0;
	}
	value = HexStringToDec(token);


	if (address == -1 || value == -1)
		return 0;

	row = address/MEMORYCOLUMN;
	col = address%MEMORYCOLUMN;
	// assign value to address
	Memory[row][col] = value;
	return 1;
}

int Power(int base, int exp){
	int val = 1, i;
	for (i=0 ; i<exp ; i++)
		val *= base;
	return val;
}

void inst_refine(char *token, char *inst_refined){
	int inst_refined_length = 0;
	int i;
	for (i=0 ; token[i]!='\0' ; i++){
		inst_refined[inst_refined_length++] = token[i];
	}	
	token = strtok(NULL, " ,\t");
	while (token != NULL){
		inst_refined[inst_refined_length++] = ' ';
		for (i=0 ; token[i]!='\0' ; i++){
			inst_refined[inst_refined_length++] = token[i];
		}
		token = strtok(NULL, " ,\t");
		if (token == NULL)
			break;
		inst_refined[inst_refined_length++] = ',';
	}
	inst_refined[inst_refined_length] = '\0';
}

int DecStringToDec(char* token){
	int i;
	int val = 0;
	for (i=0 ; i<strlen(token) ; i++){
		val += (token[i]-48) * Power(10, (strlen(token)-1-i));
	}
	return val;
}

int HexStringToDec(char *token){
	int i;
	int val = 0;
	for (i=0 ; i<strlen(token) ; i++){
		if (token[i] >= '0' && token[i] <= '9')
			val += (token[i]-48) * Power(16, (strlen(token)-1-i));
		else if (token[i] >= 'A' && token[i] <= 'F')
			val += (token[i]-55) * Power(16, (strlen(token)-1-i));
		else if (token[i] >= 'a' && token[i] <= 'f')
			val += (token[i]-87) * Power(16, (strlen(token)-1-i));
		else {// error
			printf("Not hexadecimal format. Returned -1\n");
			return -1;	
		}
	}
	return val;
}

int inst_fill(char *inst_refined, int CommaCount){
// Return : 1 if valid instruction
//          0 if unvalid instruction
	int start, end, value;
	int start_row, start_column, end_row, end_column;
	int i;
	char inst_temp[30];
	char *token;
		
	strcpy(inst_temp, inst_refined);
	token = strtok(inst_temp, " ,");

	// exception handling
	if (CommaCount != 2){
		printf("Wrong instruction(comma error)\n");
		return 0;
	}

	// extract start
	token = strtok(NULL, " ,\t");
	if (token == NULL){
		printf("Not enough value\n");
		return 0;
	}
	start = HexStringToDec(token);

	// extract end
	token = strtok(NULL, " ,\t");
	if (token == NULL){
		printf("Not enough value\n");
		return 0;
	}
	end = HexStringToDec(token);

	// extract value
	token = strtok(NULL, " \t");
	if (token == NULL){
		printf("Not enough value\n");
		return 0;
	}
	value = HexStringToDec(token);

	// exception handling
	if (start > end){
		printf("'Start' bigger than 'end'\n");
		return 0;
	}
	if (start == -1 || end == -1 || value == -1)
		return 0;

	start_row = start/MEMORYCOLUMN;
	start_column = start%MEMORYCOLUMN;
	end_row = end/MEMORYCOLUMN;
	end_column = end%MEMORYCOLUMN;
	
	// print only one line
	if (start_row == end_row){
		for (i=start_column ; i<=end_column; i++)
			Memory[start_row][i] = value;
	}
	// print more than one line
	else {
		while (start_row <= end_row){
			for (i=0 ; i<16 ; i++){
				if (start_row == start/MEMORYCOLUMN && i<start_column)
					continue;
				else if (start_row == end/MEMORYCOLUMN && i>end_column)
					continue;
				else
					Memory[start_row][i] = value;
			}
			start_row++;
		}
	}
	return 1;
}

void inst_reset(){
	int i, j;
	for (i=0 ; i<MEMORYROW ; i++)
		for (j=0 ; j<MEMORYCOLUMN ; j++)
			Memory[i][j] = 0;
	printf("Memory reset\n");
	return;
}

void opcodeFileOpen(){
	FILE *fp;
	char str[50], mnemonic[15];
	char *token;
	int code, count, type, index;

	fp = fopen("opcode.txt", "r");
	if (!fp){
		puts("File open error");
		exit(1);
	}

	while (fgets(str, 50, fp)) {
		token = strtok(str, " \t\n");
		if (token != NULL){
			// extract 'code'
			code = HexStringToDec(token);
			count = 0;
			while (token != NULL){
				// extract  'mnemonic' 'type'
				token = strtok(NULL, " \t");
				count++;
				if (count == 1)
					strcpy(mnemonic, token);
				else if (count == 2){
					type = atoi(token);
					break;
				}
			}
			index = HashFunc(mnemonic);
			// append
			opCodeList_append(code, mnemonic, type, index);
		}
	}
	fclose(fp);
}

int HashFunc(char *mnemonic){
	int i, sum=0;
	for (i=0 ; i<strlen(mnemonic) ; i++)
		sum += mnemonic[i] * 2 + 15;
	return sum % 20;
}

void opCodeList_append(int code, char* mnemonic, int type, int index){
	opPointer newNode = (opPointer)malloc(sizeof(opCode));
	// manage linkedlist
	newNode->next = table[index].head;
	table[index].head = newNode;
	table[index].count++;
	// manage new node
	strcpy(newNode->mnemonic, mnemonic);
	newNode->code = code;
	newNode->type = type;
}


int inst_opcode(char* mnemonic, int index){
	// find mnemonic in given table
	opPointer iter;
	iter = table[index].head;
	while (iter != NULL){
		if (strcmp(iter->mnemonic, mnemonic) == 0){
			printf("opcode is %X\n", iter->code);		
			return 1;
		}
		iter = iter->next;
	}
	return 0;
}

void inst_opcodelist(){
	// print hashtable
	int i;
	opPointer iter;

	for (i=0 ; i<20 ; i++){
		printf("%2d : ", i);
		iter = table[i].head;
		while (iter != NULL){
			printf("[%6s, %2X]", iter->mnemonic, iter->code);
			iter = iter->next;
			if (iter != NULL)
				printf(" -> ");
		}
		printf("\n");
	}
}

int CommaCheck(char *inst){
	int i=0, count = 0;
	while (inst[i] != '\0'){
		if (inst[i++] == ',')
			count++;
	}
	return count;
}
