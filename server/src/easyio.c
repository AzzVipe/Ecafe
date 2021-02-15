#include <stdio.h>
#include <stdlib.h>

#include <easyio.h>

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
char* GetString(void)
{
	unsigned int size = 100, c, written;
	char *s, *news;

	s = (char *)malloc(sizeof(char) * size);
	written = 0;
	while((c = getchar()) != '\n')
	{
		/* written + 2 because, for null byte space and indext starts from 0 */
		if(written + 2 >= size)
		{
			news = (char *)realloc(s, size += size);

			/* recovering on failure */
			if(news)
				s = news; // s gets address of newly allocated mem
			else
				s = s;    // s gets old address, recovering from failure
		}
		s[written++] = c;
	}

	s[written] = '\0';

	return s;
}

/**
 * GetInt() function returns first occurence of int
 * from the input stream stdin.
 * This function solves the issue of scanf(), when
 * integer is expecting (%d) and non number is entered
 */
int GetInt(void)
{
	int num;
	char* s;
	s = GetString();
	sscanf(s, "%d", &num);
	return num;
}

/**
 * GetDouble() function returns first occurence of double
 * from the input stream stdin.
 * This function solves the issue of scanf(), when
 * double is expecting (%f) and non number is entered
 */
double GetDouble(void)
{
	double num;
	char* s;
	s = GetString();
	sscanf(s, "%lf", &num);
	return num;
}



