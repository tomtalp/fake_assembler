#define KEYWORD_ENCODING_TYPE_BITS 2
#define KEYWORD_ADDRESSING_MODE_BITS 3
#define KEYWORD_OP_CODE_BITS 4

#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024
#define MAX_SYMBOL_NAME_LENGTH 31


/* Representation of a keyword in INT's */ 
typedef struct memKeyword {
    unsigned int encodingType : KEYWORD_ENCODING_TYPE_BITS;
    unsigned int targetAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
    unsigned int opCode : KEYWORD_OP_CODE_BITS;
    unsigned int sourceAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
} memKeyword;

/* Representation of a memKeyword as a String (char array of size 12)*/
typedef struct memKeywordBinaryString{
    char keyword[MAX_KEYWORD_BINARY_LENGTH];
} memKeywordBinaryString;

/* The code instructions section - contains the actual code instructions  */
typedef struct codeInstructionsTable {
    int instructionCount;
    memKeywordBinaryString *rows[MAX_INSTRUCTIONS];
} codeInstructionsTable;

/* The data definition section - contains the data counter (DC) and an array of data value
encoded in binary*/
typedef struct dataDefinitionsTables {
    int dataCounter;
    char *rows[MAX_INSTRUCTIONS];
} dataDefinitionsTables;

typedef struct symbolTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memoryAddress;
    int isExternal;
    int isInstruction; /* Is the symbol pointing to a code instruction statement? */
    struct symbolTableNode *next;
} symbolTableNode;

typedef struct symbolTable {
    symbolTableNode *head;
    int symbolsCounter;
} symbolTable;

void initSymbolTable(symbolTable *symbTable);
void printSymbolTable(symbolTable *tb);
void addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, int isExternal, int isInstruction);
void printDataTable(dataDefinitionsTables *dataTable);
