#define MAX_SYMBOL_NAME_LENGTH 31
#define MAX_RESERVED_KEYWORD_SIZE 7
#define RESERVED_KEYWORDS_COUNT 27
#define DATA_DECLARATION_TYPES_COUNT 2
#define OP_CODES_COUNT 16

enum rowTypes {
    CODE_INSTRUCTION,
    DATA_DECLARATION,
    EXTERNAL_DECLARATION,
    ENTRY_DECLARATION
};

enum dataDefTypes {
    DATA_TYPE,
    STRING_TYPE
};

typedef struct dataTypeInfo{
    char *dataDefName;
    int dataDefCodeNum;
} dataTypeInfo;

typedef struct opCode {
    char *opCodeName;
    int opCodeNum;
} opCode;

struct CODE_INSTRUCTION_ROW_METADATA {
    opCode oc;
};

struct DATA_DECLARATION_ROW_METADATA {
    enum dataDefTypes type;
};

typedef struct parsedRow {
    int originalLineNum;
    int isValidRow;
    int hasSymbol;
    char symbolName[MAX_SYMBOL_NAME_LENGTH];
    enum rowTypes rowType;
    union {
        struct CODE_INSTRUCTION_ROW_METADATA codeRowMetadata;
        struct DATA_DECLARATION_ROW_METADATA dataRowMetadata; 
    } rowMetadata;
} parsedRow;