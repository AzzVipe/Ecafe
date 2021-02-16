#ifndef EASYIO_H
#define EASYIO_H

#define ATTR_RESET "\x1b[0m"
#define FG_RED     "\x1b[31m"
#define FG_GREEN   "\x1b[32m"
#define FG_BLUE    "\x1b[34m"
#define FG_YELLOW  "\x1b[93m"
#define BOLD       "\x1b[1m"
#define CLEAR_SCR_HOME     "\x1b[2J\x1b[H"

typedef char * String;

int  GetString(char **line, FILE *fp);

int  GetStringv2(const char *buf, String *line);

/**
 * GetString_v3(): reads string from stdin and intelligently
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

char* GetStringv3(void);

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

int GetCharacter();


/**
 * Usage: int n = GetLine( char *dest, FILE *src)
 *
 */

int GetLine(char *line, FILE *file);



#endif