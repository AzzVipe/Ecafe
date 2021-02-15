#ifndef EASYIO_H
#define EASYIO_H


typedef char * String;

/**
 * char* GetString(void);
 * int GetInt(void);
 * double GetDouble(void);
 */

/**
 * GetString(): reads string from stdin and intelligently
 * stores it in annonymos memory and finally returns the
 * char* to calling function.
 *
 * It doesn't include trailing newline character. String
 * will be stored in memory this way:
 * +---+---+---+---+---+----+
 * | H | E | L | L | O | \0 |
 * +---+---+---+---+---+----+
 *
 * Usage: String s = GetString()
 */
char* GetString(void);

/**
 * GetInt() function returns first occurence of int
 * from the input stream stdin.
 * This function solves the issue of scanf(), when
 * integer is expecting (%d) and non number is entered
 */
int GetInt(void);

/**
 * GetDouble() function returns first occurence of double
 * from the input stream stdin.
 * This function solves the issue of scanf(), when
 * double is expecting (%f) and non number is entered
 */
double GetDouble(void);

#endif
