#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <easyio.h>


int GetString(char **line, FILE *fp)
{
	unsigned int size = 100, c, written;
	char *s, *news;

	s = (String)malloc(sizeof(char) * size);
	written = 0;
	while((c = getc(fp)) != '\n' && c != EOF)
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
	*line = s;

	if (written == 0 && c == '\0')
		written = -1;

	return written;
}

int GetStringv2(const char *buf, String *line)
{
	unsigned int size = 100, c;
	long int written;
	char *s, *news;

	written = 0;
	if (buf == NULL)
		return written;

	s = (char *)malloc(sizeof(char) * size);
	int i = 0;
	while((c = buf[i++]) != '\n' && c != '\0')
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
	*line = s;

	if (written == 0 && c == '\0')
		written = -1;

	return written;
}


char* GetStringv3(void)
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



int GetInt(void)
{
	int num;
	char *s;
	GetString(&s, stdin);
	sscanf(s, "%d", &num);
	return num;
}

double GetDouble(void)
{
	double num;
	char *s;
	GetString(&s, stdin);
	sscanf(s, "%lf", &num);
	return num;
}


int GetChararcter()
{
	struct termios ts_orig, ts;
	//int fd = open_tty();
	int status;
	int ch = 0;

	// Backing original_TTY_DEVICE config
	status = tcgetattr(STDIN_FILENO, &ts_orig);
	if(status == -1)
	{
		fprintf(stderr, "ERROR: Failed to backup Terminal settings");
	}

	// Setting bit mask in c_lflag
	ts = ts_orig;
	ts.c_lflag &= ~(ICANON | ECHO);

	// Setting attribute
	tcsetattr(STDIN_FILENO, TCSANOW, &ts);

	// Testing input
	int c = getchar();

	// Restoring original settings
	status = tcsetattr(STDIN_FILENO, TCSANOW, &ts_orig);
	if(status != 0)
	{
		fputs("ERROR: Failed to restore original settings\n", stderr);
	}

	return c;
}

int GetLine(char *line, FILE *file)
{
	int c, i;
	i = 0;
	while ((c = getc(file)) != '\n' && c != EOF)
	{
		line[i++] = c;
	}

	line[i] = '\0';

	return i;
}