#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DECLARATION_TYPES_COUNT 2
#define OP_CODES_COUNT 16
#define MAX_INSTRUCTION_LENGTH 256 /* TODO - IS THIS OK? */

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

enum ROW_TYPES {
    CODE_INSTRUCTION,
    DATA_DECLARATION,
    EXTERNAL_DECLARATION,
    ENTRY_DECLARATION
};

enum LEGAL_ADDRESSING_MODES {
    IMMEDIATE_MODE = 1,
    DIRECT_MODE = 3,
    REGISTER_MODE = 5
};

enum DATA_DEF_TYPES {
    DATA_TYPE,
    STRING_TYPE
};

typedef struct dataTypeInfo{
    char *dataDefName;
    int dataDefCodeNum;
} dataTypeInfo;

// typedef struct opCodeLegalArgs {

// };

typedef int (*opCodeValidator)(char *, int);

typedef struct opCode {
    char *opCodeName;
    int opCodeNum;
    opCodeValidator validator;
} opCode;

struct CODE_INSTRUCTION_ROW_METADATA {
    opCode oc;
    char *srcOperand;
    char *destOperand;
};

struct DATA_DECLARATION_ROW_METADATA {
    enum DATA_DEF_TYPES type;
    char rawData[MAX_INSTRUCTION_LENGTH];
};

typedef struct parsedRow {
    int originalLineNum;
    int isValidRow;
    int hasSymbol;
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    enum ROW_TYPES rowType;
    union {
        struct CODE_INSTRUCTION_ROW_METADATA codeRowMetadata;
        struct DATA_DECLARATION_ROW_METADATA dataRowMetadata; 
    } rowMetadata;
} parsedRow;