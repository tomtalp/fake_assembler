#ifndef DATA_STRUCTS_H
#define DATA_STRUCTS_H

#define KEYWORD_ENCODING_TYPE_BITS 2
#define KEYWORD_ADDRESSING_MODE_BITS 3
#define KEYWORD_OP_CODE_BITS 4

#define MAX_KEYWORD_BINARY_LENGTH 12
#define MAX_INSTRUCTIONS 1024
#define MAX_SYMBOL_NAME_LENGTH 31
#define BASE_MEM_ADDRESS 100
#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DECLARATION_TYPES_COUNT 2
#define OP_CODES_COUNT 16
#define MAX_INSTRUCTION_LENGTH 80
#define REGISTERS_COUNT 8
#define REGISTER_KEYWORD_LENGTH 3
#define REGISTER_OPERAND_BINARY_SIZE 5
#define ENCODING_TYPES_MAX_LENGTH 2
#define CODE_INSTRUCTION_KEYWORD_DATA_SIZE 10
#define FILENAME_MAX_LENGTH 80

#define MOV 0
#define CMP 1
#define ADD 2
#define SUB 3
#define NOT 4
#define CLR 5
#define LEA 6
#define INC 7
#define DEC 8
#define JMP 9
#define BNE 10
#define RED 11
#define PRN 12
#define JSR 13
#define RTS 14
#define STOP 15

/* Representation of a keyword in INT's */ 
typedef struct memKeyword {
    unsigned int encodingType : KEYWORD_ENCODING_TYPE_BITS;
    unsigned int destAddressingMode : KEYWORD_ADDRESSING_MODE_BITS;
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
    // memKeywordBinaryString *rows[MAX_INSTRUCTIONS];
    char *rows[MAX_INSTRUCTIONS];
} codeInstructionsTable;

/* The data definition section - contains the data counter (DC) and an array of data value
encoded in binary*/

typedef struct dataDefinitionNode {
    char binaryData[MAX_KEYWORD_BINARY_LENGTH+1];
    struct dataDefinitionNode *next;
} dataDefinitionNode;

typedef struct dataDefinitionsTable {
    dataDefinitionNode *head;
    int dataCounter;
} dataDefinitionsTable;

/* Types of legal symbols in our symbol table */
enum SYMBOL_TYPES {
    DATA_SYMBOL,
    INSTRUCTION_SYMBOL,
    EXTERNAL_SYMBOL,
    ENTRY_SYMBOL
};

/* 
    Relocation table - A linked list containing data for symbols that later need to be relocated
    For example, if we have a label declaration - `NAME: .data 123`
    And then that label is used, for example `mov NAME, @r2` then we'll store a record for NAME and this
    command in the relocation table, and later we can replace NAME with the correct memory address
 */
typedef struct relocationTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memAddress;
    struct relocationTableNode *next;
} relocationTableNode;

typedef struct relocationTable {
    relocationTableNode *head;
    int relocationVariablesCounter;
} relocationTable;

/* A symbol table for holding data on symbols detected in the code  */
typedef struct symbolTableNode {
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    int memoryAddress;
    enum SYMBOL_TYPES symbolType;
    struct symbolTableNode *next;
} symbolTableNode;

typedef struct symbolTable {
    symbolTableNode *head;
    int symbolsCounter;
    relocationTable *relocTable; /* Store the reloc table inside the symbol table, since they're always used together */
} symbolTable;

/* Type of rows that we can encounter*/
enum ROW_TYPES {
    CODE_INSTRUCTION,
    DATA_DECLARATION,
    EXTERNAL_DECLARATION,
    ENTRY_DECLARATION
};

/* Addressing modes for code operands */
enum LEGAL_ADDRESSING_MODES {
    NO_OPERAND = 0,
    IMMEDIATE_MODE = 1,
    DIRECT_MODE = 3,
    REGISTER_MODE = 5
};

/* Types of legal data definitions - string defs or int defs*/
enum DATA_DEF_TYPES {
    DATA_TYPE,
    STRING_TYPE
};

/* Encoding types for our binary keywords */
enum ENCODING_TYPES {
    ABSOLUTE_TYPE = 0,
    EXTERNAL_TYPE = 1,
    RELOCATABLE_TYPE = 2
};

/* Metadata for a data def type */
typedef struct dataTypeInfo {
    char *dataDefName;
    int dataDefCodeNum;
} dataTypeInfo;

/* Operation code information */
typedef struct opCode {
    char *opCodeName;
    int opCodeNum;
} opCode;

/* Metadata for code instructions - info on each operand and their types, and the op code */
struct CODE_INSTRUCTION_ROW_METADATA {
    opCode oc;
    char srcOperand[MAX_SYMBOL_NAME_LENGTH];
    enum LEGAL_ADDRESSING_MODES srcOperandType;
    char destOperand[MAX_SYMBOL_NAME_LENGTH];
    enum LEGAL_ADDRESSING_MODES destOperandType;
};

/* data declaration metadata - data type & raw data */
struct DATA_DECLARATION_ROW_METADATA {
    enum DATA_DEF_TYPES type;
    char rawData[MAX_INSTRUCTION_LENGTH];
};

/* extern/entry declaration label name */
struct EXTERN_ENTRY_DECLARATION_ROW_METADATA {
    char labelName[MAX_SYMBOL_NAME_LENGTH];
};

/* Parsing errors we might encounter */
typedef enum {
    NO_ERROR,
    INVALID_OPCODE_ERROR,
    SYMBOL_TOO_LONG,
    SYMBOL_STARTS_WITH_NON_ALPHA,
    SYMBOL_CONTAINS_INVALID_CHAR,
    INVALID_DATA_DEF_TYPE,
    INVALID_REGISTER_NAME,
    INVALID_LABEL_DECLARATION,
    LABEL_DECLARATION_TOO_LONG,
    DATA_STRING_DECLARATION_MISSING_QUOTES,
    EXTRANEOUS_TEXT_AFTER_STRING_QUOTES,
    ILLEGAL_COMMA_IN_DATA_DECLARATION,
    ILLEGAL_POSITIVITY_SIGN_IN_DATA_DECLARATION,
    ILLEGAL_DATA_DECLARATION_CHARACTER,
    ILLEGAL_DATA_DECLARATION_EXTRANEOUS_COMMA,
    ILLEGAL_DATA_DECLARATION_EXTRANEOUS_SPACE,
    ILLEGAL_DATA_DECLARATION_EMPTY_DATA,
    ILLEGAL_CODE_OPERANDS,
    MISSING_COMMA_BETWEEN_OPERANDS,
    MULTIPLE_COMMAS_BETWEEN_OPERANDS,
    NO_TEXT_AFTER_COMMA,
    EXTRANEOS_TEXT_AFTER_OPERANDS,
    EXTRANEOUS_TEXT_AFTER_EXTERN_ENTRY,
    DUPLICATE_SYMBOL_DECLARATION,
    UNKNOWN_ERROR,
} ERROR_TYPES;

/* 
    A struct for holding data on a parsed row. Once we encounter a row, we want to extract all
    of the info we can for that row, so we'll store it in a special struct
*/
typedef struct parsedRow {
    char fileName[FILENAME_MAX_LENGTH];
    int originalLineNum;
    ERROR_TYPES errorType;
    int hasSymbol;
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    enum ROW_TYPES rowType;
    union {
        struct CODE_INSTRUCTION_ROW_METADATA codeRowMetadata;
        struct DATA_DECLARATION_ROW_METADATA dataRowMetadata; 
        struct EXTERN_ENTRY_DECLARATION_ROW_METADATA externRowMetadata;
        struct EXTERN_ENTRY_DECLARATION_ROW_METADATA entryRowMetadata;
    } rowMetadata;
} parsedRow;

/* Store a linked list of parsed rows, so that we can iterate over them once again in the 2nd iteration */
typedef struct parsedRowNode {
    parsedRow pr;
    struct parsedRowNode *next;
} parsedRowNode;

typedef struct parsedRowList {
    parsedRowNode *head;
    int parsedRowsCounter;
} parsedRowList;

void initSymbolTable(symbolTable *symbTable);
void printSymbolTable(symbolTable *tb);
int addNodeToSymbolTable(symbolTable *tb, char *symbolName, int memAddress, enum SYMBOL_TYPES symbolType);
void initDataTable(dataDefinitionsTable *dataTable);
void addNodeToDataTable(dataDefinitionsTable *tb, int num);
void printDataTable(dataDefinitionsTable *dataTable);
void printCodeTable(codeInstructionsTable *codeTable);
void updateSymbolTableAddresses(symbolTable *tb, int baseMemAddress, int instructionCount);
void addToRelocationsTable(relocationTable *relocTable, char *varName, int memAddress);
void printRelocTable(relocationTable *relocTable);
symbolTableNode *fetchFromSymbTableByName(symbolTable *tb, char *symbolName);

#endif