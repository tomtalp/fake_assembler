#ifndef ERRORS_H
#define ERRORS_H

#define RED_PRINT          "\033[31m"           /* Red format*/
#define RESET_PRINT        "\033[0m"            /* Normal string format print */
#define BOLD_RED_PRINT     "\033[1m\033[31m"    /* Bold Red format */
#define BOLD_WHITE_PRINT   "\033[1m\033[37m"    /* Bold White */

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
} ERROR_TYPES;

#endif