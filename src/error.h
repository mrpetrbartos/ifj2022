/**
 * @file error.h
 * @author Petr Bartoš (xbarto0g)
 * @brief Header file for error reporting.
 */

#ifndef H_ERROR
#define H_ERROR

#define ERR_LEXICAL_AN 1       // lexical analysis error
#define ERR_SYNTAX_AN 2        // syntax analysis error
#define ERR_UNDEF_REDEF_FUNC 3 // undefined/redefined function
#define ERR_FUNC_VAR 4         // function is passed/returns incorrect (number of) variables
#define ERR_UNDEF_VAR 5        // undefined variable
#define ERR_RUNTIME_EXPR 6     // missing/abundant expression
#define ERR_TYPE_COMP 7        // incompatible types
#define ERR_OTHER 8            // other semantic errors
#define ERR_INTERNAL 99        // internal compiler error

/**
 * @brief Prints a error message to stderr.
 *
 * @param line Line on which the error occured at.
 * @param ch Position of the error-causing statement on the line.
 * @param message Message to be displayed.
 */
void printError(int line, int ch, char *message);

#endif
