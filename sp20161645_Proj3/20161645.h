#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define MEMORYCOLUMN 16
#define MEMORYROW 65536
#define MEMORYSIZE 1048576

// ------------- global variable and structure -------------------
// Project 1
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

// Project 2
typedef struct _SYMTAB{
	char symbol[30];
	int loc;
}SYMTAB;

SYMTAB symbolTable[500];
int symbolTable_size = 0;
int Modif_Rec[300] = {0};


// Project 3
int progaddr;

typedef struct _ExteralSymbolTable{
	char CSECT[6];
	char SYMBOL[6];
	int address, length;
	int flag; // 0 if CEST, 1 if SYMBOL
} ExternalSymbolTable;

ExternalSymbolTable ESTAB[500];
int ESTAB_size = 0;

int bp[30];
int bp_size = 0;

// --------------- function declaration ----------------

// Project 1
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


// Project 2
int inst_type(char* filename);
int inst_assemble(char* filename);
int inst_symbol();
int assembler_pass1(char* filename, char* prog_name, int* loc, int* locidx, int* prog_length);
int assembler_pass2(char* filename, FILE* lstfile, int* loc, int locidx);
int make_obj_file(char* filename, FILE* lstfile, FILE* objfile, char* prog_name, int prog_length, int* loc);
int symbolTable_search(char* token);
int directive_BYTE(char* token);
int find_opcode_format(char* mnemonic);
int find_register_number(char* token);
int XBPE_to_int(int x, int b, int p, int e);


// Project 3
int inst_progaddr(char* addr);
int inst_loader(char* objfile1, char* objfile2, char* objfile3);
int loader_pass1(char* objfile, int* CSADDR);
int loader_pass2(char* objfile, int START_ADDR);
void print_ESTAB();
int search_ESTAB(char* symbol);
