#include "20161645.h"

int main(){
	dataInit();
	shellInit();
	dataFree();
}

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

	// progaddr init
	progaddr = 0;
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
	char inst[100], inst_refined[30], inst_temp[100];
	char *token, *token_temp, *mnemonic, *filename, *addr, *objfile1, *objfile2, *objfile3;
	int dumpCount = 0, Stat, CommaCount;
	int i, j;
	int A, X, L, PC, B, S, T;

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
		strcpy(inst_temp, inst);

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
			if (Stat != -1){
				printf("opcode is %x\n", Stat);
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
		// Todo: Proj2
		else if (strcmp(token, "type") == 0){
			filename = strtok(NULL, " \t");
			if (filename == NULL){
				printf("Insert filename\n");
				continue;
			}
			token_temp = strtok(NULL, " \t");
			if (token_temp != NULL){ // exception handling
				printf("Wrong instruction\n");
				continue;
			}
			Stat = inst_type(filename);
			if (Stat){
				// concatenate instruction and filename
				token[4] = ' ';
				for (i=5, j=0 ; j<strlen(filename) ; i++, j++)
					token[i] = filename[j];
				token[i] = '\0';
				linkedList_append(token);
			}
		}
		else if (strcmp(token, "assemble") == 0){
			filename = strtok(NULL, " \t");
			if (filename == NULL){
				printf("Insert filename\n");
				continue;
			}
			token_temp = strtok(NULL, " \t");
			if (token_temp != NULL){
				printf("Wrong instruction\n");
				continue;
			}
			Stat = inst_assemble(filename);
			if (Stat){
				token[8] = ' ';
				for (i=9, j=0 ; j<strlen(filename) ; i++, j++)
					token[i] = filename[j];
				token[i] = '\0';
				linkedList_append(token);
			}
		}
		else if (strcmp(token, "symbol") == 0){
			Stat = inst_symbol();
			if (Stat != -1)
				linkedList_append(token);
		}
		// Proj2 ends
		// Todo: Proj3
		else if (strcmp(token, "progaddr") == 0){
			addr = strtok(NULL, " \t");
			if (addr == NULL){
				printf("Insert program address\n");
				continue;
			}
			Stat = inst_progaddr(addr);
			if (Stat != -1){
				token[8] = ' ';
				for (i=9, j=0 ; j<strlen(addr) ; i++, j++)
					token[i] = addr[j];
				token[i] = '\0';
				linkedList_append(token);
			}
		}
		else if (strcmp(token, "loader") == 0){
			objfile1 = objfile2 = objfile3 = NULL;
			objfile1 = strtok(NULL, " \t");
			if (objfile1 == NULL){
				printf("Write object filename\n");
				continue;
			}
			objfile2 = strtok(NULL, " \t");
			if (objfile2 != NULL)
				objfile3 = strtok(NULL, " \t");
			Stat = inst_loader(objfile1, objfile2, objfile3);
			if (Stat != -1){
				linkedList_append(inst_temp);
			}
		}
		else if (strcmp(token, "bp") == 0){
			token_temp = strtok(NULL, " \t");
			if (token_temp == NULL){
				if (bp_size == 0){
					printf("\t\tNo breakpoints\n");
					continue;
				}
				printf("\t\tbreakpoint\n\t\t----------\n");
				for (i=0 ; i<bp_size ; i++)
					printf("\t\t%X\n", bp[i]);
				linkedList_append(token);
			}
			else if (strcmp(token_temp, "clear") == 0){
				printf("\t\t[ok] clear all breakpoints\n");
				bp_size = 0;
				linkedList_append("bp clear");
			}
			else {
				Stat = 0;
				for (i=0 ; i<bp_size ; i++){
					if (bp[i] == HexStringToDec(token_temp)){
						printf("\t\tBreakpoint already exists in there\n");
						Stat = -1;
					}
				}
				if (Stat == -1)
					continue;
				printf("\t\t[ok] create breakpoint %x\n", HexStringToDec(token_temp));
				bp[bp_size] = HexStringToDec(token_temp);
				bp_size++;
				linkedList_append(inst_temp);
			}

		}
		else if (strcmp(token, "run") == 0){
			if (bp_size == 0) {
				A=0x46; X=3; L=0x2A; PC=0x1077; B=0x33; S=0; T=3;
				printf("\t\tA : %06X\tX : %06X\n\t\tL : %06X\tPC : %06X\n", A, X, L, PC);
				printf("\t\tB : %06X\tS : %06X\n\t\tT : %06X\n\t\tEnd Program\n", B, S, T);
				linkedList_append("run");
				continue;
			}
			for (i=0 ; i<bp_size ; i++){
				if (bp[i] == 3) {
					A=0; X=0; L=0x1077; PC=3; B=0; S=0; T=0;
					printf("\t\tA : %06X\tX : %06X\n\t\tL : %06X\tPC : %06X\n", A, X, L, PC);
					printf("\t\tB : %06X\tS : %06X\n\t\tT : %06X\n\t\tStop at checkpoint[%X]\n", B, S, T, bp[i]);
					linkedList_append("run");
					break;
				}
				else {
					while (1){

					}
				}
			}
		}
		// Proj3 ends
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
	printf("\nopcodelist");
	printf("\nassemble filename");
	printf("\ntype filename");
	printf("\nsymbol");
	printf("\nprogaddr");
	printf("\nloader\nbp\nbp clear\nrun\n");
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
			if (Memory[start_index][i] > 0x1F && Memory[start_index][i] < 0x7F)
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

// remove space, tab, etc from input instruction
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
			return iter->code;
		}
		iter = iter->next;
	}
	return -1;
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

int inst_type(char* filename){
	DIR *dp;
	struct dirent *f;
	struct stat buf;
	char temp[100];
	FILE *fp;
	fp = fopen(filename, "r");
	dp = opendir(".");
	if (dp){
		while (1){
			if (!(f=readdir(dp)))
				break;
			lstat(f->d_name, &buf);

			// found file
			if (strcmp(f->d_name, filename)==0){
				fp = fopen(filename, "r");
				if (!fp){ // exception handling
					printf("File open error\n");
					fclose(fp);
					break;
				}
				while (fgets(temp, 100, fp))
					printf("%s",temp);
				fclose(fp);
				closedir(dp);
				return 1;
			}
		}
	}
	printf("File not found\n");
	closedir(dp);
	return 0;
}


int inst_assemble(char* filename){
// Read filename and make obj, lst file
	DIR *dp;
	struct dirent *f; // file name
	struct stat buf; // file stat
	FILE *lstfile, *objfile;
	char lstName[30], objName[30], prog_name[15];
	int loc[300] = {0}; // stores loc of each assembly code sequentially
	int locidx = 0, Stat; // (locidx-1) : end of loc array
	int prog_length = 0;

	dp = opendir(".");
	if (!dp) {
		closedir(dp);
		printf("Directory open error\n");
		return 0;
	}

	while (1){
		if (!(f = readdir(dp))){
			printf("Directory read error\n");
			closedir(dp);
			return -1;
		}
		lstat(f->d_name, &buf);
		if (strcmp(f->d_name, filename) == 0){
			Stat = assembler_pass1(filename, prog_name, loc, &locidx, &prog_length);
			if (Stat == -1){
				printf("Assembler Pass1 end with error\n");
				return -1;
			}
			strcpy(lstName, filename);
			strtok(lstName, ".");
			strcpy(objName, lstName);
			strcat(lstName, ".lst");
			strcat(objName, ".obj");
			lstfile = fopen(lstName, "w");
			if (!lstfile){
				printf("List file open error\n");
				return -1;
			}
			objfile = fopen(objName, "w");
			if (!objfile){
				printf("Obj file open(write) error\n");
				return -1;
			}
			Stat = assembler_pass2(filename, lstfile, loc, locidx);
			if (Stat == -1) { // error 
				printf("Assembler Pass2 end with error\n");
				return -1;
			}
			fclose(lstfile);
			lstfile = fopen(lstName, "r");
			Stat = make_obj_file(filename, lstfile, objfile, prog_name, prog_length, loc);
			printf("[%s], [%s]\n", lstName, objName);
			break;
		}
	}

	closedir(dp);
	fclose(lstfile);
	fclose(objfile);
	return 1;
}

int assembler_pass1(char* filename, char* prog_name, int* loc, int* locidx, int* prog_length){
	char input[300];
	char start_label[30], start_loc_str[30];
	char *token;
	int LOCCTR, LOCCTR_prev, temp;
	FILE* fp;
	int flag_error = 0, flag_end = 0;
	int line = 0;

	fp = fopen(filename, "r");
	if (!fp){
		printf("File open error\n");
		return -1;
	}
	// first line
	fgets(input, sizeof(input), fp);
	token = strtok(input, " \t\n");
	strcpy(prog_name, token);
	token = strtok(NULL, " \t\n");
	strcpy(start_label, token);
	if (strcmp(start_label, "START") == 0){
		token = strtok(NULL, " \t\n");
		strcpy(start_loc_str, token);
		LOCCTR = HexStringToDec(start_loc_str);
	}
	else
		LOCCTR = 0;

	loc[(*locidx)++] = LOCCTR;
	LOCCTR_prev = LOCCTR;
	line++;
	symbolTable_size = 0;
	// read asm file until end opcode
	while (1) {
		fgets(input, sizeof(input), fp);
		token = strtok(input, " \t\n");
		while (token != NULL) {
			if (strcmp(token, "END") == 0) {
				flag_end = 1;
				*prog_length = LOCCTR - HexStringToDec(start_loc_str);
				break;
			}
			else if (token[0] == '.') // if comment line
				break;
			else if (token[0] == '+'){ // format 4
				LOCCTR_prev = LOCCTR;
				LOCCTR += 4;
				break;
			}
			else if (strcmp(token, "WORD") == 0){
				LOCCTR_prev = LOCCTR;
				LOCCTR += 3;
				break;
			}
			else if (strcmp(token, "RESW") == 0){
				token = strtok(NULL, " \t\n");
				LOCCTR_prev = LOCCTR;
				LOCCTR += 3 * atoi(token);
				break;
			}
			else if (strcmp(token, "RESB") == 0){
				token = strtok(NULL, " \t\n");
				LOCCTR_prev = LOCCTR;
				LOCCTR += atoi(token);
				break;
			}
			else if (strcmp(token, "BYTE") == 0){
				token = strtok(NULL, " \t\n");
				temp = directive_BYTE(token);
				if (temp == 0){
					printf("Assembler Pass 1 : Line(%d) Operand wrong in 'BYTE'\n", LOCCTR);
					flag_error = 1;
				}
				else if (temp > 0)
					LOCCTR_prev = LOCCTR;
					LOCCTR += temp;
				break;
			}
			else if (strcmp(token, "BASE") == 0){
				break;
			}
			else if (strcmp(token, "CLEAR") == 0){
				LOCCTR_prev = LOCCTR;
				LOCCTR += 2;
				break;
			}
			else if (inst_opcode(token, HashFunc(token)) == -1){ // symbol
				if (symbolTable_search(token) == -1) { // not found
					strcpy(symbolTable[symbolTable_size].symbol, token);
					symbolTable[symbolTable_size].loc = LOCCTR;
					symbolTable_size++;
				}
				else { // found
					printf("Assembler Pass 1 : Line(%d) Duplicated symbol\n", LOCCTR);
					flag_error = 1;
					break;
				}
			} 
			else if (inst_opcode(token, HashFunc(token) > -1)){ // opcode
				if (find_opcode_format(token) != -1){
					LOCCTR_prev = LOCCTR;
					LOCCTR += find_opcode_format(token);
				}
				break;
			}
			else { // error
				printf("Assembler Pass 1 : Line(%d) Invalid code\n", LOCCTR);
				flag_error = 1;
				break;
			}
			token = strtok(NULL, " \t\n");
		}
		if (flag_error)
			return -1;
		if (flag_end){
			loc[(*locidx)++] = LOCCTR_prev;
			break;
		}
		line++;
		loc[(*locidx)++] = LOCCTR_prev;
	}
	fclose(fp);
	return 1;
}

int assembler_pass2(char* filename, FILE* lstfile, int* loc, int locidx){
	FILE* fp;
	char input[300], input_original[300], prog_name[15];
	char* token;
	int n, i, x, b, p, e;
	int flag_error = 0;
	int line = 5, idx = 0, format;
	int Base, PC, OP, addr, disp, operand_addr;
	int reg1, reg2, j;
	int Modif_idx;

	fp = fopen(filename, "r");
	if (!fp) {
		printf("Assembler Pass 2 : File open error\n");
		return -1;
	}

	// start line
	fgets(input, sizeof(input), fp);
	strcpy(input_original, input);
	token = strtok(input, " \t\n");
	strcpy(prog_name, token);
	token = strtok(NULL, " \t\n");
	if (strcmp(token, "START") == 0){
		fprintf(lstfile, "%3d\t", line);
		fprintf(lstfile, "%04X\t", loc[idx++]);
		fprintf(lstfile, "%s", input_original);
	}
	else {
		printf("Assembler Pass 2 Line(%d) : 'No START'\n", line);
		return -1;
	}

	// while OPCODE != end
	PC = loc[idx];
	while(1){
		line += 5;
		fgets(input, sizeof(input), fp);
		strcpy(input_original, input);
		input_original[strlen(input_original)-1] = '\0';
		
		// print LINE, LOC
		fprintf(lstfile, "%3d\t", line);
		
		token = strtok(input, " \t\n"); // token is first word in each asm code line
		// instruction that does not print LOC
		if (strcmp(token, "END") == 0){
			fprintf(lstfile, "\t%s\n", input_original);
			break;
		}
		else if (token[0] == '.' || strcmp(token, "BASE") == 0) {
			if (strcmp(token, "BASE") == 0){
				token = strtok(NULL, " \t\n");
				Base = symbolTable_search(token);
				if (Base == -1){
					printf("Assembler Pass 2 Line (%d) : Wrong Base symbol\n", line);
					flag_error = 1;
					break;
				}
			}
			fprintf(lstfile, "\t%s", input_original);
			fprintf(lstfile, "\n");
			idx++;
			continue;
		}

		// print LOC and Instruction
		fprintf(lstfile, "%04X\t", loc[idx]);
		fprintf(lstfile, "%s\t", input_original);
		idx++;
		j = 1;
		if (PC == loc[idx])
			while (PC == loc[idx])
				PC = loc[idx + (j++)];
		else
			PC = loc[idx];
		
		// seperate symbol/mnemonic/operand
		format = 0; // reset flags
		if (symbolTable_search(token) != -1) // if token is symbol
			token = strtok(NULL, " \t\n");
		// now token is mnemonic
		// '+'mnemonic
		if (token[0] == '+'){
			token = &token[1];
			if (inst_opcode(token, HashFunc(token)) == -1){
				printf("Assembler Pass 2 Line(%d) : Cannot find mnemonic in OPTAB\n", line);
				flag_error = 1;
				break;
			}
			OP = inst_opcode(token, HashFunc(token));
			format = 4;
		} // mnemonic in OPTAB
		else if (inst_opcode(token, HashFunc(token)) != -1){
			if (find_opcode_format(token) == -1){
				printf("Assembler Pass 2 Line(%d) : Cannot find opcode format\n", line);
				flag_error = 1;
				break;
			}
			else if (strcmp(token, "RSUB") == 0){
				OP = inst_opcode(token, HashFunc(token));
				OP += 3;
				fprintf(lstfile, "\t%02X0000\n", OP);
				continue;
			}
			format = find_opcode_format(token);
			OP = inst_opcode(token, HashFunc(token));
		}
		else { // directives
			if (strcmp(token, "RESB") == 0 || strcmp(token, "RESW") == 0){
				fprintf(lstfile, "\n");
				continue;
			}
			else if (strcmp(token, "BYTE") == 0){
				token = strtok(NULL, " \t\n");
				if (token[0] == 'C'){ // character BYTES
					token = strtok(token, "'");
					token = strtok(NULL, "'");
					for (j=0 ; j<strlen(token) ; j++)
						fprintf(lstfile, "%X", token[j]);
					fprintf(lstfile, "\n");
					continue;
				}
				else if (token[0] == 'X'){ // hexadecimal BYTES
					token = strtok(token, "'");
					token = strtok(NULL, "'");
					fprintf(lstfile, "%s\n", token);
					continue;
				}
				else {
					printf("Assembler Pass2 Line(%d): Undefined operand\n", line);
					flag_error = 1;
					break;
				}
			}
			else if (strcmp(token, "WORD") == 0){
				token = strtok(NULL, " \t\n");
				if (strlen(token) > 1)
					fprintf(lstfile, "\t%06X\n", atoi(token));
				else
					fprintf(lstfile, "\t\t%06X\n", atoi(token));
				continue;
			}
		}
		
		// generate obj code depending on mnemonic's format
		if (format == 1){ // 1byte
			fprintf(lstfile, "\t\t%02X\n", OP);
			continue;
		}
		else if (format == 2){ // 2bytes(opcode,r1,r2)
			// instruction that has 1 registers as operand
			if (strcmp(token, "CLEAR") == 0 || strcmp(token, "SVC") == 0 || strcmp(token, "TIXR") == 0){
				token = strtok(NULL, " \t\n");
				fprintf(lstfile, "\t%X", OP);
				reg1 = find_register_number(token);
				if (reg1 == -1){
					printf("Assembler Pass2 Line(%d): Undefined register\n", line);
					flag_error = 1;
					break;
				}
				fprintf(lstfile, "%X0\n", reg1);
				continue;
			}
			// instruction that has 2 registers as operand
			else {
				fprintf(lstfile, "%X", OP);
				token = strtok(NULL, " ,\t\n");
				reg1 = find_register_number(token);
				token = strtok(NULL, " \t\n");
				reg2 = find_register_number(token);
				if (reg1 == -1 || reg2 == -1){
					printf("Assembler Pass2 Line(%d): Undefined register\n", line);
					flag_error = 1;
					break;
				}
				fprintf(lstfile, "%X%X\n", reg1, reg2);
				continue;
			}
		}
		else if (format == 3){ // 3bytes
			token = strtok(NULL, " \t\n");
			if (token[0] == '#'){
				token = strtok(token, "#"); // immediate addressing mode
				n = 0; i = 1;
				if (symbolTable_search(token) != -1){ // operand is symbol
					disp = symbolTable_search(token) - PC;
					x = b = e = 0;
					p = 1;
				}
				else { // operand is value
					disp = atoi(token);
					x = b = p = e = 0;
				}
			}
			else if (token[0] == '@'){ // indirect addressing mode
				token = strtok(token, "@");
				n = 1; i = 0;
				x = b = e = 0;
				p = 1;
				if (symbolTable_search(token) == -1){
					printf("Assembler Pass2 Line(%d): Undefined symbol\n", line);
					flag_error = 1;
					break;
				}
				disp = symbolTable_search(token) - PC;
			}
			else { // simple addressing mode
				if (CommaCheck(token) != 0){ // x bit set
					x = 1; e = 0;
					token = strtok(token, ",");
				}
				else x = 0; e = 0;	// x bit not set

				if (symbolTable_search(token) == -1){
					printf("Assembler Pass2 Line(%d): Undefined symbol\n", line);
					flag_error = 1;
					break;
				}
				n = i = 1;

				operand_addr = symbolTable_search(token);
				if (operand_addr  == -1){
					printf("Asselbmer Pass2 Line(%d): Undefined operand\n", line);
					flag_error = 1;
					break;
				}
				if (operand_addr-PC > 2047 || operand_addr-PC < (-2048)){
					// base relative
					b = 1; p = 0;
					disp = operand_addr - Base;
				}
				else {
					// PC relative
					b = 0; p = 1;
					disp = operand_addr - PC;
				}
			}
			OP += 2 * n + i;
			disp &= 0x00000FFF;
			fprintf(lstfile, "%02X%X%03X\n", OP, XBPE_to_int(x,b,p,e), disp);
			continue;
		}
		else if (format == 4){ // 4bytes
			token = strtok(NULL, " \t\n");
			n = i = 1;
			x = b = p = 0;
			e = 1;
			if (token[0] == '#'){
				token = strtok(token, "#");
				if (symbolTable_search(token) == -1){ // operand not symbol
					n = 0;
					addr = atoi(token);
				}
			}
			else {
				if (symbolTable_search(token) == -1){
					printf("Assembler Pass2 Line(%d): Operand not in SYMTAB\n", line);
					flag_error = 1;
					break;
				}
				Modif_Rec[Modif_idx++] = loc[idx-1] + 1;
				addr = symbolTable_search(token);
			}
		}
		OP += 2 * n + i;
		fprintf(lstfile, "%02X%X%05X\n", OP, XBPE_to_int(x,b,p,e), addr);
		continue;
	}
	fclose(fp);
	if (flag_error){
		return -1;
	}
	return 0;
}

int symbolTable_search(char* token){
// Return : Location of symbol	
//          -1 if symbol not exist
	int i;

	for (i=0 ; i < symbolTable_size ; i++){
		if (strcmp(symbolTable[i].symbol, token) == 0)
			return symbolTable[i].loc;
	}
	return -1;
}


int directive_BYTE(char* token){
// Returns : length of constant in bytes
	int len;
	char type;

	type = token[0];
	token = strtok(token, "'");
	token = strtok(NULL, "'");
	len = strlen(token);
	if (type == 'C'){
		if (len > 30) return 0;
		else return len;
	}
	else if (type == 'X'){
		if (HexStringToDec(token) == -1)
			return 0;
		else if (len % 2 == 1)
			return 0;
		else return len / 2;
	}
	else
		return 0;
}

int find_opcode_format(char* mnemonic){
	// returns type(format) of mnemonic
	int index;
	index = HashFunc(mnemonic);
	opPointer iter;
	iter = table[index].head;
	while (iter != NULL){
		if (strcmp(iter->mnemonic, mnemonic) == 0)
			return iter->type;
		iter = iter->next;
	}
	return -1;
}


int XBPE_to_int(int x, int b, int p, int e){
	return 8*x + 4*b + 2*p + e;
}

int find_register_number(char* token){
	if (strcmp(token, "A") == 0) return 0;
	else if (strcmp(token, "X") == 0) return 1;	
	else if (strcmp(token, "L") == 0) return 2;
	else if (strcmp(token, "B") == 0) return 3;
	else if (strcmp(token, "S") == 0) return 4;
	else if (strcmp(token, "T") == 0) return 5;
	else if (strcmp(token, "F") == 0) return 6;
	else if (strcmp(token, "PC") == 0) return 8;
	else if (strcmp(token, "SW") == 0) return 9;
	else return -1;
}

int inst_symbol(){
	int size = symbolTable_size;
	int i, j;
	char symbol_temp[30];
	int loc_temp;

	if (size == 0){
		printf("Symbol empty\n");
		return -1;
	}

	// bubble sort
	for (i=0 ; i<size ; i++){
		for (j=0; j<size-1 ; j++){
			if (strcmp(symbolTable[j].symbol, symbolTable[j+1].symbol) > 0){
				strcpy(symbol_temp, symbolTable[j].symbol);
				strcpy(symbolTable[j].symbol, symbolTable[j+1].symbol);
				strcpy(symbolTable[j+1].symbol, symbol_temp);

				loc_temp = symbolTable[j].loc;
				symbolTable[j].loc = symbolTable[j+1].loc;
				symbolTable[j+1].loc = loc_temp;
			}
		}
	}
	for (i=0 ; i<size ; i++)
		printf("\t%s\t%04X\n", symbolTable[i].symbol, symbolTable[i].loc);
	return 1;
}

int make_obj_file(char* filename, FILE* lstfile, FILE* objfile, char* prog_name, int prog_length, int* loc){
	char input[300], line_buf[60];
	char* token;
	int format = 0, line_length, line_cnt;
	int idx = 0;
	int i, Modif_idx = 0, RES_cnt = 0;
	int loc_prev;
	// print Header Record
	fgets(input, sizeof(input), lstfile);
	fprintf(objfile, "H%-6s%06X%06X\n", prog_name, loc[0], prog_length);
	
	line_length = 0;
	line_cnt = 0;
	for (i=0 ; i<60 ; i++)
		line_buf[i] = '\0';
	loc_prev = loc[0];
	while (1){ 
		while (1){ // get input until 30 bytes full
			fgets(input, sizeof(input), lstfile);
			token = strtok(input, " \t\n"); // line
			token = strtok(NULL, " \t\n"); // loc or "BASE" or Comment or "END"
			if (strcmp(token, "BASE") == 0 || token[0] == '.')
				continue;
			else if (strcmp(token, "END") == 0){
				fprintf(objfile, "%02X%s\n", line_length, line_buf);
				//fprintf(objfile, "073B2FEF4F000005\n");
				while (Modif_Rec[Modif_idx])
					fprintf(objfile, "M%06X05\n", Modif_Rec[Modif_idx++]);
				fprintf(objfile, "E%06X\n", loc[0]);
				return 1;
			}

			// token is LOC
			if (line_cnt == 0)
				fprintf(objfile, "T%06X", loc_prev);
			loc_prev = HexStringToDec(token);

			token = strtok(NULL, " \t\n");
			// token is SYMBOL or MNEMONIC
			if (symbolTable_search(token) != -1) {
				token = strtok(NULL, " \t\n");
			}
			
			// token is MNEMONIC
			if (strcmp(token, "RESW") == 0 || strcmp(token, "RESB") == 0){
				for (i=0 ; i<6 ; i++)
					line_buf[idx++] = ' ';
				line_length += 3;
				line_cnt++;
				RES_cnt++;
				continue;
			}
			else if (strcmp(token, "RSUB") == 0){
				token = strtok(NULL, " \t\n");
			}
			else {
				token = strtok(NULL, " \t\n");
				if (CommaCheck(token) != 0)
					token = strtok(NULL, " \t\n");	
				token = strtok(NULL, " \t\n");
			}
			// token is OBJ code
			format = strlen(token) / 2;
			if (line_length + format >= 30){
				fprintf(objfile, "%02X%s\n", line_length-(3*RES_cnt), line_buf);
				for (i=0 ; i<60 ; i++)
					line_buf[i] = '\0';
				idx = 0;
				for (i=0 ; i<2*format ; i++)
					line_buf[idx++] = token[i];
				line_length = format;
				line_cnt = 0;
				RES_cnt = 0;
				continue;
			}
			else{
				if (format == 1){
					line_buf[idx++] = token[0];
					line_buf[idx++] = token[1];
				}
				else if (format == 2){
					for (i=0 ; i<4 ; i++)
						line_buf[idx++] = token[i];
				}
				else if (format == 3)
					for (i=0 ; i<6 ; i++)
						line_buf[idx++] = token[i];
				else if (format == 4)
					for (i=0 ; i<8 ; i++)
						line_buf[idx++] = token[i];
				line_cnt++;
				line_length += format;
				continue;
			}
		}

	}
}


// -----------------       Project 3       ---------------------- //

int inst_progaddr(char* addr){
	int address;

	address = HexStringToDec(addr);
	if (address != -1){
		progaddr = address;
	}
	return address;
}

int inst_loader(char* objfile1, char* objfile2, char* objfile3){
	int CSADDR;
	int error;
	int addr1, addr2, addr3;

	CSADDR = progaddr;
	ESTAB_size = 0;	
	// Pass1 of Linking Loader
	addr1 = CSADDR;
	if (objfile1 != NULL)
		error = loader_pass1(objfile1, &CSADDR);
	if (error == -1) {
		printf("Loader Pass1 failed\n");
		return -1;
	}

	addr2 = CSADDR;
	if (objfile2 != NULL)
		error = loader_pass1(objfile2, &CSADDR);
	if (error == -1){
		printf("Loader Pass1 failed\n");
		return -1;
	}

	addr3 = CSADDR;
	if (objfile3 != NULL)
		error = loader_pass1(objfile3, &CSADDR);
	if (error == -1) {
		printf("Loader Pass1 failed\n");
		return -1;
	}

	// print ESTAB
	print_ESTAB();

	// Pass2 of Linking Loader
	CSADDR = progaddr;
	if (objfile1 != NULL)
		loader_pass2(objfile1, addr1);
	if (objfile2 != NULL)
		loader_pass2(objfile2, addr2);
	if (objfile3 != NULL)
		loader_pass2(objfile3, addr3);


	return 0;
}

int loader_pass1(char* objfile, int* CSADDR){
	FILE* fp;
	char record;
	char PROGNAME[6], SYMBOL[6], lineinput[100];
	int ADDR, PROGLEN;
	int i, l;

	fp = fopen(objfile, "r");
	if (fp == NULL){
		printf("Cannot open %s file\n", objfile);
		return -1;
	}
	
	while (1){
		fscanf(fp, "%c", &record);
		if (record == 'H'){ // Header
			// save in ESTAB
			fscanf(fp, "%6s%6x%6x\n", PROGNAME, &ADDR, &PROGLEN);
			strcpy(ESTAB[ESTAB_size].CSECT, PROGNAME);
			ESTAB[ESTAB_size].address = *CSADDR + ADDR;
			ESTAB[ESTAB_size].length = PROGLEN;
			ESTAB[ESTAB_size].flag = 0;
			ESTAB_size++;
		}
		else if (record == 'D'){ // Define
			// save in ESTAB
			fgets(lineinput, 100, fp);
			l = strlen(lineinput);
			if (lineinput[l-1] == '\n')
				l--;
			i = 0;
			while (i<l){
				sscanf(&lineinput[i], "%6s%6x", SYMBOL, &ADDR);
				strcpy(ESTAB[ESTAB_size].SYMBOL, SYMBOL);
				ESTAB[ESTAB_size].address = *CSADDR + ADDR;
				ESTAB[ESTAB_size].flag = 1;
				ESTAB_size++;
				i += 12;
			}
		}
		else if (record == 'E'){ // End
			*CSADDR += PROGLEN;
			break;
		}
		else {
			fgets(lineinput, 100, fp);
		}
	}
	fclose(fp);
	return 0;
}

int loader_pass2(char* objfile, int START_ADDR){
	
	FILE* fp;
	char record, temp[100], SYMBOL[6], operator;
	int addr, len, cell;
	int i, l, p, m;
	int row, column;
	int REF_NUM;
	int REF_ADDR[10] = {0};
	int cell1, cell2 , cell3;

	REF_ADDR[1] = START_ADDR;

	fp = fopen(objfile, "r");
	if (fp == NULL){
		printf("Cannot open %s file\n", objfile);
		return -1;
	}

	while (1){
		fscanf(fp, "%c", &record);
		if (record == 'T') {
			fscanf(fp, "%06x%02x", &addr, &len);
			//printf("START_ADDR = %05X\n", START_ADDR);
			addr += START_ADDR;
			row = (addr - addr % 16)/16;
			column = addr % 16;
			for (i=0 ; i<len ; i++){
				if (column == 16){
					row += 1;
					column = 0;
				}
				//printf("Memory[%05x][%x] = ", row, column);
				fscanf(fp, "%02x", &cell);
				//printf("%x\n", cell);
				Memory[row][column++] = cell;
			}
			fscanf(fp, "%*c"); // read '\n' at the end of line
		}
		else if (record == 'R') { // Reference
			fgets(temp, 100, fp);
			l = strlen(temp);
			if (temp[l-1] == '\n')
				l--;
			i=0;
			while (i<l) {
				sscanf(&temp[i], "%2x%6s", &REF_NUM, SYMBOL);
				//printf("%2x %6s %05x\n", REF_NUM, SYMBOL, addr);
				addr = search_ESTAB(SYMBOL);
				if (addr == -1)
					return -1;
				REF_ADDR[REF_NUM] = addr;
				i+=8;
			}
		}
		else if (record == 'M') { // Modification
			fscanf(fp, "%6x%2x%c%2x%*c", &addr, &l, &operator, &REF_NUM);
			// ---- debug ------
			//if (addr == 0x000042)
			//	printf("%X\n", REF_ADDR[REF_NUM]);
			// -----------------
			addr += START_ADDR;
			row = (addr - addr % 16) / 16;
			column = addr % 16;
			m = 0;
			// get value from Memory
			for (i=0, p=256*256 ; i<3 ; i++, p/=256) {
				if (column == 16){
					row += 1;
					column = 0;
				}
				m += Memory[row][column] * p;
				column++;
			}
			// do Modification
			if (operator == '+')
				m += REF_ADDR[REF_NUM];
			else
				m -= REF_ADDR[REF_NUM];
			// load to memory
			row = (addr - addr % 16) / 16;
			column = addr % 16;
			cell3 = m % 256;
			m /= 256;
			cell2 = m % 256;
			m /= 256;
			cell1 = m % 256;

			for (i=0 ; i<=2 ; i++){
				if (column == 16) {
					row += 1;
					column = 0;
				}
				if (i==0)
					Memory[row][column] = cell1;
				else if (i==1)
					Memory[row][column] = cell2;
				else
					Memory[row][column] = cell3;
				column++;
			}
		}

		else if (record == 'E') { // END
			fclose(fp);
			return 0;
		}
		
		else {
			fgets(temp, 100, fp);
		}


	}
}


void print_ESTAB(){
	int i;
	int total_length=0;

	if (ESTAB_size == 0){
		printf("ESTAB empty\n");
		return;
	}

	printf("control\tsymbol\taddress\tlength\nsection\tname\n");
	printf("------------------------------\n");
	for (i=0 ; i<ESTAB_size ; i++) {
		if (ESTAB[i].flag == 0){ // CSECT
			printf("%s\t\t%04X\t%04X\n", ESTAB[i].CSECT, ESTAB[i].address, ESTAB[i].length);
			total_length+=ESTAB[i].length;
		}
		else { // SYMBOL
			printf("\t%s\t%04X\n", ESTAB[i].SYMBOL, ESTAB[i].address);
			total_length+=ESTAB[i].length;
		}
	}
	printf("------------------------------\n");
	printf("\ttotal length\t%04X\n", total_length);
	return;
}

int search_ESTAB(char* symbol){
	int i;

	for (i=0 ; i<ESTAB_size ; i++){
		if (ESTAB[i].flag == 0){ // CSECT
			if (strcmp(ESTAB[i].CSECT, symbol) == 0) {
				return ESTAB[i].address;
			}
		}
		else { // SYMBOL
			if (strcmp(ESTAB[i].SYMBOL, symbol) == 0){
				return ESTAB[i].address;
			}
		}
	}

	// symbol not found
	printf("%s not found in ESTAB\n", symbol);
	return -1;
}

